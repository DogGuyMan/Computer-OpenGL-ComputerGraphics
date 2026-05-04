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
#include <memory>

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
		float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	};

	Metahuman::PODTransform g_xform;
	Metahuman::UVTransform g_uv;
	Metahuman::ResourceManagement g_rm = Metahuman::ResourceManagement();

} // namespace

void InitApplication(int &argc, char **argv, const AppConfig &cfg)
{
	glutInitWindowSize(cfg.width, cfg.height);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow(cfg.title);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
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
void HandleMouse(int, int, int , int );
void HandleMotion(int, int );

int main(int argc, char **argv)
{
	/* 1. Application = GLUT/GL 부트스트랩 + 메인 루프 */
	InitApplication(argc, argv, {800, 600, "Example"});

	/* 2. 데모 데이터 설정 */
	display.SetWidth(800);
	display.Setheight(600);
	camera.SetFovSpeed(10.0);

	/* 2-1. 모델 등록 */
	auto keroroTexture = g_rm.LoadTexture(Metahuman::TEXTURE::TEX_KERORO_FACE);
	renderer.AddModel(std::make_unique<Metahuman::TexturedSphere>(keroroTexture));

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
	glutMouseFunc(HandleMouse);
	glutMotionFunc(HandleMotion);

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
	Metahuman::UIBeginFrame();
	Metahuman::UITransformPanel("Transform", g_xform);
	Metahuman::UIUVPanel("UV", g_uv);
	Metahuman::UIEndFrame();

	// UI 입력값을 모델에 적용
	if (auto *model = renderer.GetModel(0)) {
		model->SetTransform(g_xform);
		// TexturedSphere일 때만 UV 적용. 추후 모델이 늘어나면 picking으로 선택 모델 결정.
		if (auto *sphere = dynamic_cast<Metahuman::TexturedSphere *>(model))
			sphere->SetUV(g_uv);
	}

	glutSwapBuffers();
}

void HandleKeyboardInput(unsigned char key, int x, int y)
{
	Metahuman::UIKeyboardInput(key, x, y);
	if (!ImGui::GetIO().WantCaptureKeyboard)
		input.Dispatch(key);
	glutPostRedisplay();
}

void HandleMouse(int button, int state, int x, int y)
{
	Metahuman::UIMouse(button, state, x, y);
	if (!ImGui::GetIO().WantCaptureMouse)
		mouse.HandleMouse(button, state, x, y);
	glutPostRedisplay();
}

void HandleMotion(int x, int y)
{
	Metahuman::UIMotion(x, y);
	if (!ImGui::GetIO().WantCaptureMouse)
		mouse.HandleMotion(x, y);
	glutPostRedisplay();
}