#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include "camera.h"
#include "constants.h"
#include "display.h"
#include "inputs.h"
#include "model.h"
#include "model_imp.h"
#include "renderer.h"
#include "skybox.h"
#include "transformable.h"
#include "user_interface.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <imgui.h>
#include <memory>
#include <string>
#include <vector>

using namespace Metahuman;
using namespace std;

namespace
{
	Camera camera;
	Display display;
	KeybaordInput input;
	MouseInput mouse;
	Renderer renderer;
	Skybox g_skybox;

	struct AppConfig
	{
		int width = 800;
		int height = 600;
		const char *title = APP::DEFAULT_TITLE;
		float clearColor[4] = {0.5f, 0.5f, 0.5f, 1.0f};
	};

	enum class ModelType
	{
		KeroroHead,
		KeroroBody,
		KeroroHand,
		KeroroHat
	};

	struct ModelMeta
	{
		int id;
		ModelType type;
	};

	struct SceneModelState
	{
		ModelMeta meta;
		TransformValue xform;
		UVValue uv;
		// 아래 둘은 해당 인터페이스를 구현하는 모델에만 존재 — has* 플래그로 유무 표현.
		// (KeroroHead=둘 다 없음, KeroroBody=parametric만, KeroroHat=둘 다)
		bool hasParametric = false;
		ParametricValue parametric;
		bool hasHyperboloid = false;
		HyperboloidValue hyperboloid;
	};

	vector<ModelMeta> g_modelMetas;
	ResourceManagement g_rm = ResourceManagement();
	int g_selectedModelIndex = 0;
	int g_addModelTypeIndex = 0;
	int g_addModelId = 0;
	int g_saveStatus = -1;

	// 실행 위치(cwd)와 무관하게 프로젝트 소스 루트의 원본 json을 가리키도록 절대 경로 계산
	const char *GetSceneSavePath()
	{
#ifdef __APPLE__
		static std::string path = []() {
			std::string file_path = __FILE__;
			size_t pos = file_path.find("apps/main.cpp");
			if (pos == std::string::npos)
				pos = file_path.find("apps\\main.cpp");
			if (pos != std::string::npos)
			{
				return file_path.substr(0, pos) + "resources/scene_state.json";
			}
			return std::string("resources/scene_state.json");
		}();
		return path.c_str();
#else
		return "resources/scene_state.json";
#endif
	}

} // namespace

void InitApplication(int &argc, char **argv, const AppConfig &cfg)
{
	glutInitWindowSize(cfg.width, cfg.height);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow(cfg.title);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	// 알파 채널이 있는 텍스처(PNG RGBA)를 투명하게 합성하기 위한 표준 블렌딩.
	// 주의: 깊이 테스트와 함께 쓸 때, 반투명 모델은 불투명 모델보다 뒤에(나중에) 그려야
	//      뒤쪽 색이 이미 프레임버퍼에 있어 정상 합성된다. 모델 등록 순서가 곧 draw 순서.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(cfg.clearColor[0], cfg.clearColor[1],
	             cfg.clearColor[2], cfg.clearColor[3]);
	InitImgui();
}

void QuitApplication()
{
	exit(0);
}

void HandleWindowReshapeEvent(int, int);
void HandleDisplayEvent();
void HandleKeyboardInput(unsigned char, int, int);
void HandleKeyboardUpInput(unsigned char, int, int);
void HandleSpecialInput(int, int, int);
void HandleSpecialUpInput(int, int, int);
void HandleMouse(int, int, int, int);
void HandleMouseWheel(int, int, int, int);
void HandleMotion(int, int);
void HandlePassiveMotion(int, int);
ModelType ModelTypeFromIndex(int index);
const char *GetModelTypeLabel(ModelType type);
bool TryParseModelType(const char *label, ModelType &type);
bool AddModel(ModelType type, int id, int variant = 0);
bool IsModelIdUsed(int id);
int MakeDefaultModelId();
void MakeModelLabel(const ModelMeta &meta, char *buffer, size_t bufferSize);
bool AddModelState(const SceneModelState &state);
bool LoadSceneState(const char *path);
bool SaveSceneState(const char *path);
int ScanIntLine(const string &line, const char *format, int &value);
int ScanSizeLine(const string &line, const char *format, size_t &value);
int ScanFloatLine(const string &line, const char *format, float &x);
int ScanFloat2Line(const string &line, const char *format, float &x, float &y);
int ScanFloat3Line(const string &line, const char *format, float &x, float &y, float &z);
int ScanDouble2Line(const string &line, const char *format, double &x, double &y);
int ScanSize2Line(const string &line, const char *format, size_t &x, size_t &y);
int ScanTypeLine(const string &line, char *buffer, size_t bufferSize);

int main(int argc, char **argv)
{
	/* 1. Application = GLUT/GL 부트스트랩 + 메인 루프 */
	InitApplication(argc, argv, {800, 600, APP::WINDOW_TITLE});

	/* 2. 데모 데이터 설정 */
	display.SetWidth(800);
	display.Setheight(600);
	camera.SetFovSpeed(10.0);

	/* 2-1. 모델 등록 — KeroroHat은 알파 PNG이므로 불투명 모델(head/body) 뒤에 등록(=마지막 draw) */
	g_rm.LoadTexture(TEXTURE::TEX_KERORO_FACE);
	g_rm.LoadTexture(TEXTURE::TEX_KERORO_BODY);
	g_rm.LoadTexture(TEXTURE::TEX_KERORO_HAT);
	g_rm.LoadTexture(TEXTURE::TEX_KERORO_SKIN);

	// 스카이박스 이미지 6장 로드 및 적용
	Texture *texNX = g_rm.LoadTexture(SCENE::SKYBOX_PATH[0]);
	Texture *texNY = g_rm.LoadTexture(SCENE::SKYBOX_PATH[4]);
	Texture *texNZ = g_rm.LoadTexture(SCENE::SKYBOX_PATH[5]);
	Texture *texPX = g_rm.LoadTexture(SCENE::SKYBOX_PATH[3]);
	Texture *texPY = g_rm.LoadTexture(SCENE::SKYBOX_PATH[1]);
	Texture *texPZ = g_rm.LoadTexture(SCENE::SKYBOX_PATH[2]);

	g_skybox.SetTextures(
	    texPX ? texPX->GetTextureID() : 0, texNX ? texNX->GetTextureID() : 0,
	    texPY ? texPY->GetTextureID() : 0, texNY ? texNY->GetTextureID() : 0,
	    texPZ ? texPZ->GetTextureID() : 0, texNZ ? texNZ->GetTextureID() : 0);

	g_selectedModelIndex = 0;
	if (!LoadSceneState(GetSceneSavePath()))
	{
		g_saveStatus = 0;
		int id = 0;
		AddModel(ModelType::KeroroHead, id++);    // 디폴트로 만들어져야 하는것.
		AddModel(ModelType::KeroroBody, id++);    // 디폴트로 만들어져야 하는것.
		AddModel(ModelType::KeroroHand, id++, 0); // 왼손 (손이 Hat보다 먼저 그려져야 함)
		AddModel(ModelType::KeroroHand, id++, 1); // 오른손
		AddModel(ModelType::KeroroHat, id++);     // 투명 오브젝트는 가장 마지막에 렌더링
	}

	g_addModelId = MakeDefaultModelId();

	/* 3. 입력 바인딩 (정책) */
	input.BindKeyAction('a', [] { camera.Zoom(-0.5); });
	input.BindKeyAction('z', [] { camera.Zoom(0.5); });
	input.BindKeyAction('q', QuitApplication);
	input.BindKeyAction(27, QuitApplication);

	// 마우스 드래그 -> Camera Orbit 회전
	mouse.BindDragAction([](int dx, int dy) {
		// yaw (수평 회전), pitch (수직 회전)
		camera.Rotate(-dy * 0.5, dx * 0.5, 0);
	});

	/* 4. GLUT 콜백 등록 */
	glutReshapeFunc(HandleWindowReshapeEvent);
	glutDisplayFunc(HandleDisplayEvent);
	glutKeyboardFunc(HandleKeyboardInput);
	glutKeyboardUpFunc(HandleKeyboardUpInput);
	glutSpecialFunc(HandleSpecialInput);
	glutSpecialUpFunc(HandleSpecialUpInput);
	glutMouseFunc(HandleMouse);
#ifndef __APPLE__
	glutMouseWheelFunc(HandleMouseWheel);
#endif
	glutMotionFunc(HandleMotion);
	glutPassiveMotionFunc(HandlePassiveMotion);

	/* 5. ImGui 컨텍스트 초기화 */

	/* 6. 메인 루프 진입 */
	glutMainLoop();
	return 0;
}

/* GLUT 콜백 — 자유 함수에서 g_ctx 인스턴스로 위임 */
void HandleWindowReshapeEvent(int w, int h)
{
	display.Reshape(w, h, camera);
	UIReshape(w, h);
	glutPostRedisplay();
}

void HandleDisplayEvent()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 매 프레임 투영 갱신 (FOV 변경 반영)
	camera.ApplyProjection((float)display.GetAspectRatio());
	camera.ApplyView();

	// 모든 투명/불투명 오브젝트를 그리기 전, 가장 뒷배경에 스카이박스를 먼저 그립니다.
	g_skybox.Draw();

	renderer.Render(camera);

	// UI 한 프레임: Begin -> 패널들 -> End
	const int modelCount = (int)renderer.GetModelCount();
	if (modelCount > 0)
	{
		if (g_selectedModelIndex < 0)
			g_selectedModelIndex = 0;
		if (g_selectedModelIndex >= modelCount)
			g_selectedModelIndex = modelCount - 1;

		vector<array<char, 64>> modelLabelStorage;
		vector<const char *> modelLabels;
		modelLabelStorage.resize((size_t)modelCount);
		modelLabels.reserve((size_t)modelCount);
		for (int i = 0; i < modelCount; ++i)
		{
			MakeModelLabel(g_modelMetas[(size_t)i],
			               modelLabelStorage[(size_t)i].data(),
			               modelLabelStorage[(size_t)i].size());
			modelLabels.push_back(modelLabelStorage[(size_t)i].data());
		}

		// 선택 모델 = 단일 진실 공급원. 패널이 인터페이스로 직접 모델 상태를 읽고 쓴다.
		Model *model = renderer.GetModel((size_t)g_selectedModelIndex);

		UIBeginFrame();
		// Model은 ITransformable이므로 캐스트 없이 전달
		if (model)
			UITransformPanel(UI::PANEL::TRANSFORM, *model,
			                 g_selectedModelIndex, modelLabels);
		// UV / Parametric / Hyperboloid는 해당 인터페이스를 구현하는 모델에만 노출
		if (auto *uvt = dynamic_cast<IUVTransformable *>(model))
			UIUVPanel(UI::PANEL::UV, *uvt);
		if (auto *geo = dynamic_cast<IParametricTransformable *>(model))
			UIParametricPanel(UI::PANEL::PARAMETRIC, *geo);
		if (auto *hyp = dynamic_cast<IHyperboloidTransformable *>(model))
			UIHyperboloidPanel(UI::PANEL::HYPERBOLOID, *hyp);

		// 런타임중 모델 추가.
		if (UIModelAddPanel(
		        UI::PANEL::MODELS,
		        MODEL::MODEL_TYPES,
		        (int)(sizeof(MODEL::MODEL_TYPES) / sizeof(MODEL::MODEL_TYPES[0])),
		        g_addModelTypeIndex, g_addModelId))
		{
			const ModelType type = ModelTypeFromIndex(g_addModelTypeIndex);
			if (AddModel(type, g_addModelId))
				g_addModelId = MakeDefaultModelId();
		}
		const bool saveRequested = UIScenePanel(UI::PANEL::SCENE, GetSceneSavePath(), g_saveStatus);
		UIEndFrame();

		// 패널이 모델을 직접 mutate하므로 별도 "적용" 단계는 불필요.
		if (saveRequested)
			g_saveStatus = SaveSceneState(GetSceneSavePath()) ? 1 : 0;
	}

	glutSwapBuffers();
}

ModelType ModelTypeFromIndex(int index)
{
	if (index == 0)
		return ModelType::KeroroHead;
	if (index == 1)
		return ModelType::KeroroBody;
	if (index == 2)
		return ModelType::KeroroHat;
	if (index == 3)
		return ModelType::KeroroHand;
	return ModelType::KeroroHead;
}

const char *GetModelTypeLabel(ModelType type)
{
	switch (type)
	{
	case ModelType::KeroroHead:
		return MODEL::KERORO_HEAD;
	case ModelType::KeroroBody:
		return MODEL::KERORO_BODY;
	case ModelType::KeroroHat:
		return MODEL::KERORO_HAT;
	case ModelType::KeroroHand:
		return MODEL::KERORO_HAND;
	}
	return MODEL::UNKNOWN;
}

bool TryParseModelType(const char *label, ModelType &type)
{
	if (strcmp(label, MODEL::KERORO_HEAD) == 0)
	{
		type = ModelType::KeroroHead;
		return true;
	}
	if (strcmp(label, MODEL::KERORO_BODY) == 0)
	{
		type = ModelType::KeroroBody;
		return true;
	}
	if (strcmp(label, MODEL::KERORO_HAND) == 0)
	{
		type = ModelType::KeroroHand;
		return true;
	}
	if (strcmp(label, MODEL::KERORO_HAT) == 0)
	{
		type = ModelType::KeroroHat;
		return true;
	}
	return false;
}

bool IsModelIdUsed(int id)
{
	for (const auto &meta : g_modelMetas)
	{
		if (meta.id == id)
			return true;
	}
	return false;
}

int MakeDefaultModelId()
{
	for (int i = 0;; ++i)
	{
		if (!IsModelIdUsed(i))
			return i;
	}
}

void MakeModelLabel(const ModelMeta &meta, char *buffer, size_t bufferSize)
{
	snprintf(buffer, bufferSize, "%d [%s]", meta.id, GetModelTypeLabel(meta.type));
}

// variant: 같은 타입의 기본 포즈 변형 선택자(손의 좌=0/우=1 등). 생성 시점 기본 transform
// 힌트일 뿐이며 영속적 정체성이 아니다 — 포즈의 진실은 저장/로드되는 transform이다.
// 한 번 호출 = 모델 1개 = id 1개 (g_modelMetas와 renderer 모델 수의 1:1 불변식 유지).
bool AddModel(ModelType type, int id, int variant)
{
	if (id < 0 || IsModelIdUsed(id))
		return false;

	switch (type)
	{
	case ModelType::KeroroHead:
		renderer.AddModel(make_unique<KeroroHead>(
		    g_rm.textures[TEXTURE::TEX_KERORO_FACE].get()));
		break;
	case ModelType::KeroroBody:
		renderer.AddModel(make_unique<KeroroBody>(
		    g_rm.textures[TEXTURE::TEX_KERORO_BODY].get()));
		break;
	case ModelType::KeroroHand:
		renderer.AddModel(make_unique<KeroroHand>(
		    g_rm.textures[TEXTURE::TEX_KERORO_SKIN].get(), variant));
		break;
	case ModelType::KeroroHat:
		renderer.AddModel(make_unique<KeroroHat>(
		    g_rm.textures[TEXTURE::TEX_KERORO_HAT].get()));
		break;
	}

	g_modelMetas.push_back({id, type});
	g_selectedModelIndex = (int)renderer.GetModelCount() - 1;
	return true;
}

bool AddModelState(const SceneModelState &state)
{
	if (!AddModel(state.meta.type, state.meta.id))
		return false;

	// 방금 AddModel이 추가한 모델 = 마지막 인덱스. 상태는 모델에 직접 적용한다.
	const size_t index = renderer.GetModelCount() - 1;
	if (auto *model = renderer.GetModel(index))
	{
		model->SetTransform(state.xform);
		if (auto *uvTransform = dynamic_cast<IUVTransformable *>(model))
			uvTransform->SetUV(state.uv);
		// parametric -> hyperboloid 순서: Set*는 각자 build()를 호출하므로
		// 마지막 build가 두 파라미터를 모두 반영한 상태로 메쉬를 재생성한다.
		if (state.hasParametric)
		{
			if (auto *geo = dynamic_cast<IParametricTransformable *>(model))
				geo->SetParametricParams(state.parametric);
		}
		if (state.hasHyperboloid)
		{
			if (auto *hyp = dynamic_cast<IHyperboloidTransformable *>(model))
				hyp->SetHyperboloidParams(state.hyperboloid);
		}
	}
	return true;
}

bool LoadSceneState(const char *path)
{
	ifstream in(path);
	if (!in)
		return false;

	int selectedModelIndex = 0;
	SceneModelState current{};
	std::vector<SceneModelState> loadedStates;
	bool readingModel = false;
	bool inTransform = false;
	bool inUv = false;
	bool inParametric = false;
	bool inHyperboloid = false;
	string line;

	while (getline(in, line))
	{
		if (ScanIntLine(line, "  \"selectedModelIndex\": %d", selectedModelIndex) == 1)
			continue;

		size_t index = 0;
		if (ScanSizeLine(line, "      \"index\": %zu", index) == 1)
		{
			current = SceneModelState{};
			readingModel = true;
			inTransform = false;
			inUv = false;
			inParametric = false;
			inHyperboloid = false;
			continue;
		}

		if (!readingModel)
			continue;

		if (ScanIntLine(line, "      \"id\": %d", current.meta.id) == 1)
			continue;

		char typeLabel[64] = {};
		if (ScanTypeLine(line, typeLabel, sizeof(typeLabel)) == 1)
		{
			if (!TryParseModelType(typeLabel, current.meta.type))
				return false;
			continue;
		}

		if (line.find("\"transform\"") != string::npos)
		{
			inTransform = true;
			inUv = false;
			inParametric = false;
			inHyperboloid = false;
			continue;
		}
		if (line.find("\"uv\"") != string::npos)
		{
			inTransform = false;
			inUv = true;
			inParametric = false;
			inHyperboloid = false;
			continue;
		}
		if (line.find("\"parametric\"") != string::npos)
		{
			inTransform = false;
			inUv = false;
			inParametric = true;
			inHyperboloid = false;
			current.hasParametric = true;
			continue;
		}
		if (line.find("\"hyperboloid\"") != string::npos)
		{
			inTransform = false;
			inUv = false;
			inParametric = false;
			inHyperboloid = true;
			current.hasHyperboloid = true;
			continue;
		}

		if (inTransform)
		{
			if (ScanFloat3Line(line, "        \"translate\": [%f, %f, %f]",
			                   current.xform.translate.x, current.xform.translate.y, current.xform.translate.z) == 3)
				continue;
			if (ScanFloat3Line(line, "        \"rotateDeg\": [%f, %f, %f]",
			                   current.xform.eulerDeg.x, current.xform.eulerDeg.y, current.xform.eulerDeg.z) == 3)
				continue;
			if (ScanFloat3Line(line, "        \"scale\": [%f, %f, %f]",
			                   current.xform.scale.x, current.xform.scale.y, current.xform.scale.z) == 3)
				continue;
		}

		if (inUv)
		{
			if (ScanFloat2Line(line, "        \"offset\": [%f, %f]",
			                   current.uv.offset.x, current.uv.offset.y) == 2)
				continue;
			if (ScanFloat2Line(line, "        \"scale\": [%f, %f]",
			                   current.uv.scale.x, current.uv.scale.y) == 2)
				continue;
			if (ScanFloatLine(line, "        \"rotationDeg\": %f", current.uv.rotationDeg) == 1)
				continue;
		}

		if (inParametric)
		{
			if (ScanDouble2Line(line, "        \"uRange\": [%lf, %lf]",
			                    current.parametric.uStart, current.parametric.uEnd) == 2)
				continue;
			if (ScanDouble2Line(line, "        \"vRange\": [%lf, %lf]",
			                    current.parametric.vStart, current.parametric.vEnd) == 2)
				continue;
			if (ScanSize2Line(line, "        \"resolution\": [%zu, %zu]",
			                  current.parametric.uRes, current.parametric.vRes) == 2)
				continue;
		}

		if (inHyperboloid)
		{
			if (ScanFloatLine(line, "        \"radius\": %f", current.hyperboloid.radius) == 1)
				continue;
			if (ScanFloatLine(line, "        \"height\": %f", current.hyperboloid.height) == 1)
				continue;
			if (ScanFloatLine(line, "        \"shape\": %f", current.hyperboloid.shape) == 1)
				continue;
		}

		if (line.rfind("    }", 0) == 0)
		{
			loadedStates.push_back(current);
			readingModel = false;
			inTransform = false;
			inUv = false;
			inParametric = false;
			inHyperboloid = false;
		}
	}

	for (const auto &state : loadedStates)
	{
		if (!AddModelState(state))
			return false;
	}

	if (renderer.GetModelCount() == 0)
		return false;
	if (selectedModelIndex < 0)
		selectedModelIndex = 0;
	if (selectedModelIndex >= (int)renderer.GetModelCount())
		selectedModelIndex = (int)renderer.GetModelCount() - 1;
	g_selectedModelIndex = selectedModelIndex;
	return true;
}

int ScanIntLine(const string &line, const char *format, int &value)
{
#ifdef _MSC_VER
	return sscanf_s(line.c_str(), format, &value);
#else
	return sscanf(line.c_str(), format, &value);
#endif
}

int ScanSizeLine(const string &line, const char *format, size_t &value)
{
#ifdef _MSC_VER
	return sscanf_s(line.c_str(), format, &value);
#else
	return sscanf(line.c_str(), format, &value);
#endif
}

int ScanFloatLine(const string &line, const char *format, float &x)
{
#ifdef _MSC_VER
	return sscanf_s(line.c_str(), format, &x);
#else
	return sscanf(line.c_str(), format, &x);
#endif
}

int ScanFloat2Line(const string &line, const char *format, float &x, float &y)
{
#ifdef _MSC_VER
	return sscanf_s(line.c_str(), format, &x, &y);
#else
	return sscanf(line.c_str(), format, &x, &y);
#endif
}

int ScanFloat3Line(const string &line, const char *format, float &x, float &y, float &z)
{
#ifdef _MSC_VER
	return sscanf_s(line.c_str(), format, &x, &y, &z);
#else
	return sscanf(line.c_str(), format, &x, &y, &z);
#endif
}

int ScanDouble2Line(const string &line, const char *format, double &x, double &y)
{
#ifdef _MSC_VER
	return sscanf_s(line.c_str(), format, &x, &y);
#else
	return sscanf(line.c_str(), format, &x, &y);
#endif
}

int ScanSize2Line(const string &line, const char *format, size_t &x, size_t &y)
{
#ifdef _MSC_VER
	return sscanf_s(line.c_str(), format, &x, &y);
#else
	return sscanf(line.c_str(), format, &x, &y);
#endif
}

int ScanTypeLine(const string &line, char *buffer, size_t bufferSize)
{
#ifdef _MSC_VER
	return sscanf_s(line.c_str(), "      \"type\": \"%63[^\"]\"", buffer, (unsigned)bufferSize);
#else
	return sscanf(line.c_str(), "      \"type\": \"%63[^\"]\"", buffer);
#endif
}

bool SaveSceneState(const char *path)
{
	ofstream out(path);
	if (!out)
		return false;

	out << "{\n";
	out << "  \"version\": 2,\n";
	out << "  \"selectedModelIndex\": " << g_selectedModelIndex << ",\n";
	out << "  \"models\": [\n";
	for (size_t i = 0; i < g_modelMetas.size(); ++i)
	{
		const auto &meta = g_modelMetas[i];
		// 모델 = 단일 진실 공급원. 각 상태를 인터페이스로 조회한다.
		// transform/uv/parametric/hyperboloid 블록은 해당 인터페이스 구현 모델에만 출력.
		Model *model = renderer.GetModel(i);
		const TransformValue &xform = model->GetPODTransform();
		auto *uvt = dynamic_cast<IUVTransformable *>(model);
		auto *geo = dynamic_cast<IParametricTransformable *>(model);
		auto *hyp = dynamic_cast<IHyperboloidTransformable *>(model);
		out << "    {\n";
		out << "      \"index\": " << i << ",\n";
		out << "      \"id\": " << meta.id << ",\n";
		out << "      \"type\": \"" << GetModelTypeLabel(meta.type) << "\",\n";
		out << "      \"transform\": {\n";
		out << "        \"translate\": [" << xform.translate.x << ", " << xform.translate.y << ", " << xform.translate.z << "],\n";
		out << "        \"rotateDeg\": [" << xform.eulerDeg.x << ", " << xform.eulerDeg.y << ", " << xform.eulerDeg.z << "],\n";
		out << "        \"scale\": [" << xform.scale.x << ", " << xform.scale.y << ", " << xform.scale.z << "]\n";
		out << "      }" << ((uvt || geo || hyp) ? ",\n" : "\n");
		if (uvt)
		{
			const UVValue &uv = uvt->GetUV();
			out << "      \"uv\": {\n";
			out << "        \"offset\": [" << uv.offset.x << ", " << uv.offset.y << "],\n";
			out << "        \"scale\": [" << uv.scale.x << ", " << uv.scale.y << "],\n";
			out << "        \"rotationDeg\": " << uv.rotationDeg << "\n";
			out << "      }" << ((geo || hyp) ? ",\n" : "\n");
		}
		if (geo)
		{
			const auto &p = geo->GetParametricParams();
			out << "      \"parametric\": {\n";
			out << "        \"uRange\": [" << p.uStart << ", " << p.uEnd << "],\n";
			out << "        \"vRange\": [" << p.vStart << ", " << p.vEnd << "],\n";
			out << "        \"resolution\": [" << p.uRes << ", " << p.vRes << "]\n";
			out << "      }" << (hyp ? ",\n" : "\n");
		}
		if (hyp)
		{
			const auto &h = hyp->GetHyperboloidParams();
			out << "      \"hyperboloid\": {\n";
			out << "        \"radius\": " << h.radius << ",\n";
			out << "        \"height\": " << h.height << ",\n";
			out << "        \"shape\": " << h.shape << "\n";
			out << "      }\n";
		}
		out << "    }" << (i + 1 < g_modelMetas.size() ? "," : "") << "\n";
	}
	out << "  ]\n";
	out << "}\n";
	return (bool)out;
}

void HandleKeyboardInput(unsigned char key, int x, int y)
{
	UIKeyboardInput(key, x, y);
	if (!ImGui::GetIO().WantCaptureKeyboard)
		input.Dispatch(key);
	glutPostRedisplay();
}

void HandleKeyboardUpInput(unsigned char key, int x, int y)
{
	UIKeyboardUpInput(key, x, y);
	glutPostRedisplay();
}

void HandleSpecialInput(int key, int x, int y)
{
	UISpecialInput(key, x, y);
	glutPostRedisplay();
}

void HandleSpecialUpInput(int key, int x, int y)
{
	UISpecialUpInput(key, x, y);
	glutPostRedisplay();
}

void HandleMouse(int button, int state, int x, int y)
{
	UIMouse(button, state, x, y);
	if (!ImGui::GetIO().WantCaptureMouse || state == GLUT_UP)
		mouse.HandleMouse(button, state, x, y);
	else
		mouse.CancelDrag();
	glutPostRedisplay();
}

void HandleMouseWheel(int button, int dir, int x, int y)
{
	UIMouseWheel(button, dir, x, y);
	glutPostRedisplay();
}

void HandleMotion(int x, int y)
{
	UIMotion(x, y);
	if (!ImGui::GetIO().WantCaptureMouse)
		mouse.HandleMotion(x, y);
	else
		mouse.CancelDrag();
	glutPostRedisplay();
}

void HandlePassiveMotion(int x, int y)
{
	UIMotion(x, y);
	glutPostRedisplay();
}
