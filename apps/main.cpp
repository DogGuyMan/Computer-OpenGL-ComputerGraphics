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
#include "transformable.h"
#include "user_interface.h"
#include <array>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <imgui.h>
#include <memory>
#include <string>
#include <vector>

namespace
{
	Metahuman::Camera camera;
	Metahuman::Display display;
	Metahuman::KeybaordInput input;
	Metahuman::MouseInput mouse;
	Metahuman::Renderer renderer;

	struct AppConfig
	{
		int width = 800;
		int height = 600;
		const char *title = "Application";
		float clearColor[4] = {0.5f, 0.5f, 0.5f, 1.0f};
	};

	enum class ModelType
	{
		KeroroHead,
		KeroroBody,
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
		Metahuman::TransformValue xform;
		Metahuman::UVValue uv;
		// 아래 둘은 해당 인터페이스를 구현하는 모델에만 존재 — has* 플래그로 유무 표현.
		// (KeroroHead=둘 다 없음, KeroroBody=parametric만, KeroroHat=둘 다)
		bool hasParametric = false;
		Metahuman::ParametricValue parametric;
		bool hasHyperboloid = false;
		Metahuman::HyperboloidValue hyperboloid;
	};

	std::vector<ModelMeta> g_modelMetas;
	Metahuman::ResourceManagement g_rm = Metahuman::ResourceManagement();
	int g_selectedModelIndex = 0;
	Metahuman::Texture *g_keroroFaceTexture = nullptr;
	Metahuman::Texture *g_keroroBodyTexture = nullptr;
	Metahuman::Texture *g_keroroHatTexture = nullptr;
	const char *const g_modelTypes[] = {"KeroroHead", "KeroroBody", "KeroroHat"};
	const char *g_sceneSavePath = "resources/scene_state.json";
	int g_addModelTypeIndex = 0;
	int g_addModelId = 0;
	int g_saveStatus = -1;

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
	Metahuman::InitImgui();
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
bool AddModel(ModelType type, int id);
bool IsModelIdUsed(int id);
int MakeDefaultModelId();
void MakeModelLabel(const ModelMeta &meta, char *buffer, size_t bufferSize);
bool AddModelState(const SceneModelState &state);
bool LoadSceneState(const char *path);
bool SaveSceneState(const char *path);
int ScanIntLine(const std::string &line, const char *format, int &value);
int ScanSizeLine(const std::string &line, const char *format, size_t &value);
int ScanFloatLine(const std::string &line, const char *format, float &x);
int ScanFloat2Line(const std::string &line, const char *format, float &x, float &y);
int ScanFloat3Line(const std::string &line, const char *format, float &x, float &y, float &z);
int ScanDouble2Line(const std::string &line, const char *format, double &x, double &y);
int ScanSize2Line(const std::string &line, const char *format, size_t &x, size_t &y);
int ScanTypeLine(const std::string &line, char *buffer, size_t bufferSize);

int main(int argc, char **argv)
{
	/* 1. Application = GLUT/GL 부트스트랩 + 메인 루프 */
	InitApplication(argc, argv, {800, 600, "Example"});

	/* 2. 데모 데이터 설정 */
	display.SetWidth(800);
	display.Setheight(600);
	camera.SetFovSpeed(10.0);

	/* 2-1. 모델 등록 — KeroroHat은 알파 PNG이므로 불투명 모델(head/body) 뒤에 등록(=마지막 draw) */
	g_keroroFaceTexture = g_rm.LoadTexture(Metahuman::TEXTURE::TEX_KERORO_FACE);
	g_keroroBodyTexture = g_rm.LoadTexture(Metahuman::TEXTURE::TEX_KERORO_BODY);
	g_keroroHatTexture = g_rm.LoadTexture(Metahuman::TEXTURE::TEX_KERORO_HAT);
	AddModel(ModelType::KeroroHead, 0);
	AddModel(ModelType::KeroroBody, 1);
	AddModel(ModelType::KeroroHat, 2);
	g_selectedModelIndex = 0;
	if (!LoadSceneState(g_sceneSavePath))
		g_saveStatus = 0;

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
	Metahuman::UIReshape(w, h);
	glutPostRedisplay();
}

void HandleDisplayEvent()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 매 프레임 투영 갱신 (FOV 변경 반영)
	camera.ApplyProjection((float)display.GetAspectRatio());
	camera.ApplyView();
	renderer.Render(camera);

	// UI 한 프레임: Begin -> 패널들 -> End
	const int modelCount = (int)renderer.GetModelCount();
	if (modelCount > 0)
	{
		if (g_selectedModelIndex < 0)
			g_selectedModelIndex = 0;
		if (g_selectedModelIndex >= modelCount)
			g_selectedModelIndex = modelCount - 1;

		std::vector<std::array<char, 64>> modelLabelStorage;
		std::vector<const char *> modelLabels;
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
		Metahuman::Model *model = renderer.GetModel((size_t)g_selectedModelIndex);

		Metahuman::UIBeginFrame();
		// Model은 ITransformable이므로 캐스트 없이 전달
		if (model)
			Metahuman::UITransformPanel("Transform", *model,
			                            g_selectedModelIndex, modelLabels);
		// UV / Parametric / Hyperboloid는 해당 인터페이스를 구현하는 모델에만 노출
		if (auto *uvt = dynamic_cast<Metahuman::IUVTransformable *>(model))
			Metahuman::UIUVPanel("UV", *uvt);
		if (auto *geo = dynamic_cast<Metahuman::IParametricTransformable *>(model))
			Metahuman::UIParametricPanel("Parametric", *geo);
		if (auto *hyp = dynamic_cast<Metahuman::IHyperboloidTransformable *>(model))
			Metahuman::UIHyperboloidPanel("Hyperboloid", *hyp);

		// 런타임중 모델 추가.
		if (Metahuman::UIModelAddPanel("Models", g_modelTypes, (int)(sizeof(g_modelTypes) / sizeof(g_modelTypes[0])),
		                               g_addModelTypeIndex, g_addModelId))
		{
			const ModelType type = ModelTypeFromIndex(g_addModelTypeIndex);
			if (AddModel(type, g_addModelId))
				g_addModelId = MakeDefaultModelId();
		}
		const bool saveRequested = Metahuman::UIScenePanel("Scene", g_sceneSavePath, g_saveStatus);
		Metahuman::UIEndFrame();

		// 패널이 모델을 직접 mutate하므로 별도 "적용" 단계는 불필요.
		if (saveRequested)
			g_saveStatus = SaveSceneState(g_sceneSavePath) ? 1 : 0;
	}

	glutSwapBuffers();
}

ModelType ModelTypeFromIndex(int index)
{
	if (index == 1)
		return ModelType::KeroroBody;
	if (index == 2)
		return ModelType::KeroroHat;
	return ModelType::KeroroHead;
}

const char *GetModelTypeLabel(ModelType type)
{
	switch (type)
	{
	case ModelType::KeroroHead:
		return "KeroroHead";
	case ModelType::KeroroBody:
		return "KeroroBody";
	case ModelType::KeroroHat:
		return "KeroroHat";
	}
	return "Unknown";
}

bool TryParseModelType(const char *label, ModelType &type)
{
	if (std::strcmp(label, "KeroroHead") == 0)
	{
		type = ModelType::KeroroHead;
		return true;
	}
	if (std::strcmp(label, "KeroroBody") == 0)
	{
		type = ModelType::KeroroBody;
		return true;
	}
	if (std::strcmp(label, "KeroroHat") == 0)
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
	std::snprintf(buffer, bufferSize, "%d [%s]", meta.id, GetModelTypeLabel(meta.type));
}

bool AddModel(ModelType type, int id)
{
	if (id < 0 || IsModelIdUsed(id))
		return false;

	switch (type)
	{
	case ModelType::KeroroHead:
		renderer.AddModel(std::make_unique<Metahuman::KeroroHead>(g_keroroFaceTexture));
		break;
	case ModelType::KeroroBody:
		renderer.AddModel(std::make_unique<Metahuman::KeroroBody>(g_keroroBodyTexture));
		break;
	case ModelType::KeroroHat:
		renderer.AddModel(std::make_unique<Metahuman::KeroroHat>(g_keroroHatTexture));
		break;
	}

	g_modelMetas.push_back({id, type});
	// g_xforms.emplace_back(std::make_unique<Metahuman::PODTransform>());
	// g_uvs.emplace_back(std::make_unique<Metahuman::UVTransform>());
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
		if (auto *uvTransform = dynamic_cast<Metahuman::IUVTransformable *>(model))
			uvTransform->SetUV(state.uv);
		// parametric → hyperboloid 순서: Set*는 각자 build()를 호출하므로
		// 마지막 build가 두 파라미터를 모두 반영한 상태로 메쉬를 재생성한다.
		if (state.hasParametric)
		{
			if (auto *geo = dynamic_cast<Metahuman::IParametricTransformable *>(model))
				geo->SetParametricParams(state.parametric);
		}
		if (state.hasHyperboloid)
		{
			if (auto *hyp = dynamic_cast<Metahuman::IHyperboloidTransformable *>(model))
				hyp->SetHyperboloidParams(state.hyperboloid);
		}
	}
	return true;
}

bool LoadSceneState(const char *path)
{
	std::ifstream in(path);
	if (!in)
		return false;

	int selectedModelIndex = 0;
	SceneModelState current{};
	bool readingModel = false;
	bool inTransform = false;
	bool inUv = false;
	bool inParametric = false;
	bool inHyperboloid = false;
	std::string line;

	while (std::getline(in, line))
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

		if (line.find("\"transform\"") != std::string::npos)
		{
			inTransform = true;
			inUv = false;
			inParametric = false;
			inHyperboloid = false;
			continue;
		}
		if (line.find("\"uv\"") != std::string::npos)
		{
			inTransform = false;
			inUv = true;
			inParametric = false;
			inHyperboloid = false;
			continue;
		}
		if (line.find("\"parametric\"") != std::string::npos)
		{
			inTransform = false;
			inUv = false;
			inParametric = true;
			inHyperboloid = false;
			current.hasParametric = true;
			continue;
		}
		if (line.find("\"hyperboloid\"") != std::string::npos)
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
			if (!AddModelState(current))
				return false;
			readingModel = false;
			inTransform = false;
			inUv = false;
			inParametric = false;
			inHyperboloid = false;
		}
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

int ScanIntLine(const std::string &line, const char *format, int &value)
{
#ifdef _MSC_VER
	return sscanf_s(line.c_str(), format, &value);
#else
	return std::sscanf(line.c_str(), format, &value);
#endif
}

int ScanSizeLine(const std::string &line, const char *format, size_t &value)
{
#ifdef _MSC_VER
	return sscanf_s(line.c_str(), format, &value);
#else
	return std::sscanf(line.c_str(), format, &value);
#endif
}

int ScanFloatLine(const std::string &line, const char *format, float &x)
{
#ifdef _MSC_VER
	return sscanf_s(line.c_str(), format, &x);
#else
	return std::sscanf(line.c_str(), format, &x);
#endif
}

int ScanFloat2Line(const std::string &line, const char *format, float &x, float &y)
{
#ifdef _MSC_VER
	return sscanf_s(line.c_str(), format, &x, &y);
#else
	return std::sscanf(line.c_str(), format, &x, &y);
#endif
}

int ScanFloat3Line(const std::string &line, const char *format, float &x, float &y, float &z)
{
#ifdef _MSC_VER
	return sscanf_s(line.c_str(), format, &x, &y, &z);
#else
	return std::sscanf(line.c_str(), format, &x, &y, &z);
#endif
}

int ScanDouble2Line(const std::string &line, const char *format, double &x, double &y)
{
#ifdef _MSC_VER
	return sscanf_s(line.c_str(), format, &x, &y);
#else
	return std::sscanf(line.c_str(), format, &x, &y);
#endif
}

int ScanSize2Line(const std::string &line, const char *format, size_t &x, size_t &y)
{
#ifdef _MSC_VER
	return sscanf_s(line.c_str(), format, &x, &y);
#else
	return std::sscanf(line.c_str(), format, &x, &y);
#endif
}

int ScanTypeLine(const std::string &line, char *buffer, size_t bufferSize)
{
#ifdef _MSC_VER
	return sscanf_s(line.c_str(), "      \"type\": \"%63[^\"]\"", buffer, (unsigned)bufferSize);
#else
	return std::sscanf(line.c_str(), "      \"type\": \"%63[^\"]\"", buffer);
#endif
}

bool SaveSceneState(const char *path)
{
	std::ofstream out(path);
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
		Metahuman::Model *model = renderer.GetModel(i);
		const Metahuman::TransformValue &xform = model->GetPODTransform();
		auto *uvt = dynamic_cast<Metahuman::IUVTransformable *>(model);
		auto *geo = dynamic_cast<Metahuman::IParametricTransformable *>(model);
		auto *hyp = dynamic_cast<Metahuman::IHyperboloidTransformable *>(model);
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
			const Metahuman::UVValue &uv = uvt->GetUV();
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
	Metahuman::UIKeyboardInput(key, x, y);
	if (!ImGui::GetIO().WantCaptureKeyboard)
		input.Dispatch(key);
	glutPostRedisplay();
}

void HandleKeyboardUpInput(unsigned char key, int x, int y)
{
	Metahuman::UIKeyboardUpInput(key, x, y);
	glutPostRedisplay();
}

void HandleSpecialInput(int key, int x, int y)
{
	Metahuman::UISpecialInput(key, x, y);
	glutPostRedisplay();
}

void HandleSpecialUpInput(int key, int x, int y)
{
	Metahuman::UISpecialUpInput(key, x, y);
	glutPostRedisplay();
}

void HandleMouse(int button, int state, int x, int y)
{
	Metahuman::UIMouse(button, state, x, y);
	if (!ImGui::GetIO().WantCaptureMouse || state == GLUT_UP)
		mouse.HandleMouse(button, state, x, y);
	else
		mouse.CancelDrag();
	glutPostRedisplay();
}

void HandleMouseWheel(int button, int dir, int x, int y)
{
	Metahuman::UIMouseWheel(button, dir, x, y);
	glutPostRedisplay();
}

void HandleMotion(int x, int y)
{
	Metahuman::UIMotion(x, y);
	if (!ImGui::GetIO().WantCaptureMouse)
		mouse.HandleMotion(x, y);
	else
		mouse.CancelDrag();
	glutPostRedisplay();
}

void HandlePassiveMotion(int x, int y)
{
	Metahuman::UIMotion(x, y);
	glutPostRedisplay();
}
