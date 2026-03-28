#include "entry.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	// glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	// GLUT_DOUBLE : 더블 버퍼링 사용.
	// 	화면에 보이는 front buffer와 그리는 back buffer 두 개를 두고,
	// 	그리기 완료 후 glutSwapBuffers()로 교체. 깜빡임(flickering) 방지

	// GLUT_RGB : RGB 색상 모드 사용 (인덱스 색상 모드가 아닌)

	glutCreateWindow("Example");
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glutReshapeFunc(SJH::Chapter1::Reshape);
	glutDisplayFunc(SJH::Chapter1::Display1);
	// glutDisplayFunc(SJH::Chapter1::Display2);
	// glutDisplayFunc(SJH::Chapter1::Display3);
	// glutDisplayFunc(SJH::Chapter1::Display4);
	// glutDisplayFunc(SJH::Chapter1::Display5);
	glutMainLoop();
}
