#include "chapter4.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <iostream>
#include <spdlog/spdlog.h>

void Tutorials::Chapter4::Callbacks::HandleWindowReshapeEvent(int w, int h)
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
		"Tutorials::Chapter4::Callbacks::HandleWindowReshapeEvent\n"
		"Current Width and Height : {0}, {1}", w, h
	);
}

void Tutorials::Chapter4::Callbacks::HandleDisplayEvent(void)
{
	// 라인 굵기 범위 저장용
	GLfloat size[2] = {0,};
	glClear(GL_COLOR_BUFFER_BIT); // 색상 버퍼 초기화
	// ✅ glGetFloatv, glLineWidth는 glBegin/glEnd 밖에서 호출해야 함 ✅
	glGetFloatv(GL_LINE_WIDTH_RANGE, size); // 라인 굵기 최소/최대 범위 조회
	std::cout << "size[0] : " << size[0] << '\n'
		  << "size[1] : " << size[1] << std::endl; // 최소/최대 굵기 출력
	glLineWidth(size[1]);				   // 라인 굵기를 최대값으로 설정

	glBegin(GL_LINES); // 선 그리기 시작
	{
		// ! ❌ 안됨 std::cout << "size[0] : " << size[0] << '\n'
		// ! ❌ 안됨 	  << "size[1] : " << size[1]
		// ! ❌ 안됨 	  << std::endl;
		// ! ❌ 안됨 glGetFloatv(GL_LINE_WIDTH_RANGE, size);
		// ! ❌ 안됨 glLineWidth(size[1]);
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f); // 초록색 (시작점)
		glVertex3f(-1.0f, 0.0f, 0.0f);	   // 선의 시작점 (왼쪽 끝)
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f); // 빨간색 (끝점)
		glVertex3f(1.0f, 0.0f, 0.0f);	   // 선의 끝점 (오른쪽 끝)
	}
	glEnd();   // 선 그리기 종료
	glFlush(); // 렌더링 명령 즉시 실행
}

void Tutorials::Chapter4::Callbacks::HandleInputKeyboardEvent(void)
{
}