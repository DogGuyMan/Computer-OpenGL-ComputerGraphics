#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include "inputs.h"
#include "camera.h"
#include "display.h"
#include "renderer.h"
#include "model.h"
#include <memory>
#include <spdlog/spdlog.h>

struct AppContext {
	Metahuman::Camera camera;
	Metahuman::Display display;
	Metahuman::KeybaordInput input;
	Metahuman::MouseInput mouse;
	Metahuman::Renderer renderer;
};

static AppContext g_ctx;

/* GLUT 콜백 — 자유 함수에서 g_ctx 인스턴스로 위임 */
void HandleWindowReshapeEvent(int w, int h) { 
	g_ctx.display.Reshape(w, h, g_ctx.camera); 
	glutPostRedisplay();
}

void HandleDisplayEvent() { 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 매 프레임 투영 갱신 (FOV 변경 반영)
	g_ctx.camera.ApplyProjection((float)GetAspectRatio());
	g_ctx.camera.ApplyView();
	g_ctx.renderer.Render(g_ctx.camera); 

	glutSwapBuffers();
}

void HandleKeyboardInput(unsigned char key, int x, int y) {
	g_ctx.input.Dispatch(key);
	glutPostRedisplay();
}

void HandleMouse(int button, int state, int x, int y) {
	g_ctx.mouse.HandleMouse(button, state, x, y);
	glutPostRedisplay();
}

void HandleMotion(int x, int y) {
	g_ctx.mouse.HandleMotion(x, y);
	glutPostRedisplay();
}

void QuitProgram() { exit(0); }

int main(int argc, char **argv)
{
	/* 1. GLUT 초기화 */
	glutInitWindowSize(800, 600);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	/* 2. 윈도우 생성 */
	int window = glutCreateWindow("Example");

	/* 3. OpenGL 설정 */
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	/* 4. Context 설정 */
	g_ctx.display.SetWidth(800);
	g_ctx.display.Setheight(600);
	g_ctx.camera.SetFovSpeed(10.0);

	/* 4-1. 모델 등록 */
	g_ctx.renderer.AddModel(std::make_unique<Metahuman::Model>());

	/* 5. 입력 바인딩 */
	g_ctx.input.BindKeyAction('a', [&]() { g_ctx.camera.Zoom(-0.5); });
	g_ctx.input.BindKeyAction('z', [&]() { g_ctx.camera.Zoom(0.5); });
	g_ctx.input.BindKeyAction('q', QuitProgram);
	g_ctx.input.BindKeyAction(27, QuitProgram);

	// 마우스 드래그 → Camera Orbit 회전
	g_ctx.mouse.BindDragAction([&](int dx, int dy) {
		// yaw (수평 회전), pitch (수직 회전)
		g_ctx.camera.Rotate(-dy * 0.5, dx * 0.5, 0);
	});

	/* 6. GLUT 콜백 등록 */
	glutReshapeFunc(HandleWindowReshapeEvent);
	glutDisplayFunc(HandleDisplayEvent);
	glutKeyboardFunc(HandleKeyboardInput);
	glutMouseFunc(HandleMouse);
	glutMotionFunc(HandleMotion);

	/* 7. 메인 루프 */
	glutMainLoop();

	return 0;
}
