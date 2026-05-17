#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include "constants.h"
#include "camera.h"
#include "display.h"
#include "inputs.h"
#include "model.h"
#include "model_imp.h"
#include "renderer.h"
#include "transformable.h"
#include "user_interface.h"
#include <imgui.h>
#include <array>
#include <cstddef>
#include <cstdio>
#include <memory>
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

	std::vector<std::unique_ptr<Metahuman::PODTransform>> g_xforms;
	std::vector<std::unique_ptr<Metahuman::UVTransform>>  g_uvs;
	std::vector<ModelMeta> g_modelMetas;
	Metahuman::ResourceManagement g_rm = Metahuman::ResourceManagement();
	int g_selectedModelIndex = 0;
	Metahuman::Texture* g_keroroFaceTexture = nullptr;
	Metahuman::Texture* g_keroroBodyTexture = nullptr;
	Metahuman::Texture* g_keroroHatTexture = nullptr;
	const char* const g_modelTypes[] = {"KeroroHead", "KeroroBody", "KeroroHat"};
	int g_addModelTypeIndex = 0;
	int g_addModelId = 0;

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
void HandleKeyboardInput(unsigned char, int , int );
void HandleKeyboardUpInput(unsigned char, int , int );
void HandleSpecialInput(int, int , int );
void HandleSpecialUpInput(int, int , int );
void HandleMouse(int, int, int , int );
void HandleMouseWheel(int, int, int , int );
void HandleMotion(int, int );
void HandlePassiveMotion(int, int );
ModelType ModelTypeFromIndex(int index);
const char* GetModelTypeLabel(ModelType type);
bool AddModel(ModelType type, int id);
bool IsModelIdUsed(ModelType type, int id);
int MakeDefaultModelId(ModelType type);
void MakeModelLabel(const ModelMeta& meta, char* buffer, size_t bufferSize);

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
	g_keroroHatTexture  = g_rm.LoadTexture(Metahuman::TEXTURE::TEX_KERORO_HAT);
	AddModel(ModelType::KeroroHead, 0);
	AddModel(ModelType::KeroroBody, 0);
	AddModel(ModelType::KeroroHat, 0);
	g_selectedModelIndex = 0;

	// /* 2-2. per-instance Transform/UV 상태 초기화 (모델 인덱스와 1:1 매핑)
	//  *      각 모델 클래스의 Default*()로 g_xforms/g_uvs를 채우고 모델에도 즉시 적용 —
	//  *      프로그램 시작 시점부터 ImGui 값과 모델 상태가 동기화된다. */
	// const size_t modelCount = renderer.GetModelCount();
	// g_xforms.reserve(modelCount);
	// g_uvs.reserve(modelCount);
	// for (size_t i = 0; i < modelCount; ++i) {
	// 	g_xforms.emplace_back(std::make_unique<Metahuman::PODTransform>());
	// 	g_uvs.emplace_back(std::make_unique<Metahuman::UVTransform>());
	// }
	// // 등록 순서(KeroroHead=0, KeroroBody=1, KeroroHat=2)에 맞춰 초기 기본값 주입
	// if (modelCount > 0) { *g_xforms[0] = Metahuman::KeroroHead::DefaultTransform(); *g_uvs[0] = Metahuman::KeroroHead::DefaultUV(); }
	// if (modelCount > 1) { *g_xforms[1] = Metahuman::KeroroBody::DefaultTransform(); *g_uvs[1] = Metahuman::KeroroBody::DefaultUV(); }
	// if (modelCount > 2) { *g_xforms[2] = Metahuman::KeroroHat::DefaultTransform();  *g_uvs[2] = Metahuman::KeroroHat::DefaultUV();  }
	// // 모든 모델에 1회 적용 — 매 프레임 루프는 선택 모델만 갱신하므로 시작 시 전체 동기화 필요
	// for (size_t i = 0; i < modelCount; ++i) {
	// 	Metahuman::Model *m = renderer.GetModel(i);
	// 	if (!m) continue;
	// 	m->SetTransform(*g_xforms[i]);
	// 	if (auto *uvt = dynamic_cast<Metahuman::IUVTransformable *>(m))
	// 		uvt->SetUV(*g_uvs[i]);
	// }

	g_addModelId = MakeDefaultModelId(ModelTypeFromIndex(g_addModelTypeIndex));

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
	if (modelCount > 0) {
		if (g_selectedModelIndex < 0) g_selectedModelIndex = 0;
		if (g_selectedModelIndex >= modelCount) g_selectedModelIndex = modelCount - 1;

		const int editedModelIndex = g_selectedModelIndex;
		auto& xform = *g_xforms[(size_t)editedModelIndex];
		auto& uv    = *g_uvs   [(size_t)editedModelIndex];
		std::vector<std::array<char, 64>> modelLabelStorage;
		std::vector<const char*> modelLabels;
		modelLabelStorage.resize((size_t)modelCount);
		modelLabels.reserve((size_t)modelCount);
		for (int i = 0; i < modelCount; ++i) {
			MakeModelLabel(g_modelMetas[(size_t)i],
			               modelLabelStorage[(size_t)i].data(),
			               modelLabelStorage[(size_t)i].size());
			modelLabels.push_back(modelLabelStorage[(size_t)i].data());
		}

		Metahuman::UIBeginFrame();
		Metahuman::UITransformPanel("Transform", xform,
		                            g_selectedModelIndex,
		                            modelLabels);
		Metahuman::UIUVPanel("UV", uv);
		// 선택된 모델이 Parametric 곡면이면 u/v 범위·해상도 패널도 노출
		if (auto *geo = dynamic_cast<Metahuman::IParametricTransformable *>(
		        renderer.GetModel((size_t)g_selectedModelIndex)))
			Metahuman::UIParametricPanel("Parametric", *geo);
		// 선택된 모델이 Hyperboloid면 형상 파라미터 패널도 노출
		if (auto *hyp = dynamic_cast<Metahuman::IHyperboloidTransformable *>(
		        renderer.GetModel((size_t)g_selectedModelIndex)))
			Metahuman::UIHyperboloidPanel("Hyperboloid", *hyp);
	
		// 런타임중 모델 추가.
		if (Metahuman::UIModelAddPanel("Models", g_modelTypes, (int)(sizeof(g_modelTypes) / sizeof(g_modelTypes[0])),
		                               g_addModelTypeIndex, g_addModelId)) {
			const ModelType type = ModelTypeFromIndex(g_addModelTypeIndex);
			if (AddModel(type, g_addModelId))
				g_addModelId = MakeDefaultModelId(type);
		}
		Metahuman::UIEndFrame();

		// 이번 프레임에 편집한 슬롯의 상태를 적용
		if (auto *model = renderer.GetModel((size_t)editedModelIndex)) {
			model->SetTransform(xform);
			if (auto *uvTransform = dynamic_cast<Metahuman::IUVTransformable *>(model))
				uvTransform->SetUV(uv);
		}
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

const char* GetModelTypeLabel(ModelType type)
{
	switch (type) {
	case ModelType::KeroroHead:
		return "KeroroHead";
	case ModelType::KeroroBody:
		return "KeroroBody";
	case ModelType::KeroroHat:
		return "KeroroHat";
	}
	return "Unknown";
}

bool IsModelIdUsed(ModelType type, int id)
{
	for (const auto& meta : g_modelMetas) {
		if (meta.type == type && meta.id == id)
			return true;
	}
	return false;
}

int MakeDefaultModelId(ModelType type)
{
	for (int i = 0;; ++i) {
		if (!IsModelIdUsed(type, i))
			return i;
	}
}

void MakeModelLabel(const ModelMeta& meta, char* buffer, size_t bufferSize)
{
	std::snprintf(buffer, bufferSize, "%d [%s]", meta.id, GetModelTypeLabel(meta.type));
}

bool AddModel(ModelType type, int id)
{
	if (id < 0 || IsModelIdUsed(type, id))
		return false;

	switch (type) {
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
	g_xforms.emplace_back(std::make_unique<Metahuman::PODTransform>());
	g_uvs.emplace_back(std::make_unique<Metahuman::UVTransform>());
	g_selectedModelIndex = (int)renderer.GetModelCount() - 1;
	return true;
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
