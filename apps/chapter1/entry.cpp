#include "entry.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <iostream>

namespace SJH::Chapter1
{
	// window을 크기를 조절할때 호출되는 함수
	void reshape(int w, int h)
	{
		glLoadIdentity();
		glViewport(0, 0, w, h);
		gluOrtho2D(0.0, 100.0, 0.0, 100.0); // 비율은 0 또는 1 퍼센트
	}

	void display(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(1.0, 0.0, 0.0);
		// 좌단 위치 (30, 30) & 우상단 위치 (50, 50)
		glRectf(30.0, 30.0, 50.0, 50.0);
		// 그래픽 카드 메모리에 그린것을 화면에 넣어놓은 Buffer로 덮어 씌워라.
		glutSwapBuffers();
	}
} // namespace SJH::Chapter1