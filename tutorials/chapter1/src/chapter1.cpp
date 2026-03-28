#include "chapter1.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <iostream>
#include <spdlog/spdlog.h>

void Tutorials::Chapter1::Callbacks::HandleWindowReshapeEvent(int w, int h)
{
	float aspectRatio = (float)w / (float)h;
	// 이전 프레임의 변환 누적을 방지
	glLoadIdentity();
	// 윈도우 크기 변경 시 렌더링 영역 동기화 필수
	glViewport(0, 0, w, h);

	// 종횡비를 유지하면서 스케일링
	if (aspectRatio > 1.0)
	{
		// 가로가 더 김 (와이드)
		gluOrtho2D(0.0, 50.0 * aspectRatio, 0.0, 50.0);
	}
	else
	{
		// 세로가 더 김 (톨)
		gluOrtho2D(0.0, 50.0, 0.0, 50.0 / aspectRatio);
	}
	spdlog::info(
		"Tutorials::Chapter1::Callbacks::HandleWindowReshapeEvent\n"
		"Current Width and Height : {0}, {1}", w, h
	);
}

void Tutorials::Chapter1::Callbacks::HandleDisplayEvent(void)
{
	// 색상 버퍼 초기화
	glClear(GL_COLOR_BUFFER_BIT); 
	// 그래픽 카드 메모리에 그린것을 화면에 넣어놓은 Buffer로 덮어 씌워라.
	glutSwapBuffers();
}

void Tutorials::Chapter1::Callbacks::HandleInputKeyboardEvent(void)
{
}