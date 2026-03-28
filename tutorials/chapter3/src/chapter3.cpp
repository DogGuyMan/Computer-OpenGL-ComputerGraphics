#include "chapter3.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <iostream>
#include <spdlog/spdlog.h>

void Tutorials::Chapter3::Callbacks::HandleWindowReshapeEvent(int w, int h)
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
		"Tutorials::Chapter3::Callbacks::HandleWindowReshapeEvent\n"
		"Current Width and Height : {0}, {1}", w, h
	);
}

void Tutorials::Chapter3::Callbacks::HandleDisplayEvent(void)
{
	GLfloat size[2];	      // 포인트 크기 범위 저장용
	float angle;		      // 원 위의 각도
	glClear(GL_COLOR_BUFFER_BIT); // 색상 버퍼 초기화
	glColor3f(1.0f, 1.0f, 1.0f);     // 흰색 설정

	// 현재 OpenGL 구현이 지원하는 점(point) 크기의 최소/최대 범위를 조회하는 함수입니다.
	// 	* glGetFloatv — OpenGL 상태 값을 float로 가져옴
	// 	* GL_POINT_SIZE_RANGE — 점 크기 범위를 요청
	// 	* size[0] = 최소 크기, size[1] = 최대 크기
	glGetFloatv(GL_POINT_SIZE_RANGE, size);

	// glPointSize(size[0])에서 최소 크기로 점을 설정하고 있습니다.
	//	* 하드웨어마다 지원하는 범위가 다르기 때문에,
	//	* 하드코딩 대신 조회해서 쓰는 방식입니다.
	// size[0]은 가장 작은 값, size[1]은 가장 큰 값
	// 	* macOs에서는 1, 64
	std::cout << size[0] << " " << size[1] << std::endl; // 최소/최대 크기 출력
	glPointSize(size[1]);				     // 포인트 크기를 최대값으로 설정

	glBegin(GL_POINTS); // 점 그리기 시작
	for (angle = 0; angle <= 2.0f * 3.14f; angle += (2.0f * 3.14f) / 30.f) // 30개의 점으로 원 그리기
		glVertex3f(cosf(angle) * 0.5f, sinf(angle) * 0.5f, 0);	   // 반지름 0.5인 원 위의 점
	glEnd();							   // 점 그리기 종료
	glFlush();							   // 렌더링 명령 즉시 실행
}

void Tutorials::Chapter3::Callbacks::HandleInputKeyboardEvent(void)
{
}