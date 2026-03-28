#include "chapter5.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <iostream>
#include <spdlog/spdlog.h>

void Tutorials::Chapter5::Callbacks::HandleWindowReshapeEvent(int w, int h)
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
		"Tutorials::Chapter5::Callbacks::HandleWindowReshapeEvent\n"
		"Current Width and Height : {0}, {1}", w, h
	);
}

void Tutorials::Chapter5::Callbacks::HandleDisplayEvent(void)
{
	glClear(GL_COLOR_BUFFER_BIT); // 색상 버퍼 초기화
	glShadeModel(GL_SMOOTH);      // 부드러운 색상 보간 (그라디언트)
	// glShadeModel(GL_FLAT); // 가장 마지막에 넣은 색을 대표 색으로 한다.
	glBegin(GL_TRIANGLES); // 삼각형 그리기 시작
	{
		glColor3f(1.0f, 0.0f, 0.0f);    // 빨간색 (꼭짓점 1)
		glVertex3f(0.0f, 0.5f, 0.0f);  // 상단 꼭짓점
		glColor3f(0.0f, 1.0f, 0.0f);    // 초록색 (꼭짓점 2)
		glVertex3f(-0.5f, -0.5f, 0.0f); // 좌하단 꼭짓점
		glColor3f(0.0f, 0.0f, 1.0f);    // 파란색 (꼭짓점 3)
		glVertex3f(0.5f, -0.5f, 0.0f);  // 우하단 꼭짓점
	}
	glEnd();   // 삼각형 그리기 종료
	glFlush(); // 렌더링 명령 즉시 실행
}

void Tutorials::Chapter5::Callbacks::HandleInputKeyboardEvent(void)
{
}