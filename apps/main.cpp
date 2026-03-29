#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h.h>
#endif

#include "input.h"
#include "camera.h"
#include "display.h"
#include <memory>

struct AppContext {
	std::unique_ptr<Metahuman::Camera> activeCamera;
};

static AppContext g_ctx;

int main(int argc, char **argv)
{
	/* 1. GLUT 초기화 단계 */
	// glutInit 이전에 호출해야함.
	glutInitWindowSize(800, 600); 
	// 윈도우와 운영체제 컨텍스트 연결
	glutInit(&argc, argv); 

	// GLUT_DOUBLE : 더블 버퍼링 사용.
	// 	화면에 보이는 front buffer와 그리는 back buffer 두 개를 두고,
	// 	그리기 완료 후 glutSwapBuffers()로 교체. 깜빡임(flickering) 방지
	// GLUT_RGBA : RGBA 색상 모드 사용 (반투명 오브젝트 렌더링 가능)
	// GLUT_DEPTH : GLUT_DEPTH 추가 = Z-buffer 활성화 3D 물체의 깊이 정보 저장
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	/* 2. 윈도우 생성 */
	int window = glutCreateWindow("Example");

	/* 3. OpenGL 설정 */
	// 배경색 설정
	glClearColor(0.0, 0.0, 0.0, 1.0);
	// 깊이 테스트 활성화
	glEnable(GL_DEPTH_TEST); 


	return 0;
}
