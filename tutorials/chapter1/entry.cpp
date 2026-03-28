#include "entry.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <iostream>
#include <math.h>

namespace SJH::Chapter1
{
	// window을 크기를 조절할때 호출되는 함수
	void Reshape(int w, int h)
	{
		glLoadIdentity();		  // 현재 행렬을 단위 행렬로 초기화
		glViewport(0, 0, w, h);		  // 뷰포트를 윈도우 크기에 맞게 설정
		gluOrtho2D(0.0, 50.0, 0.0, 50.0); // 비율은 0 또는 1 퍼센트
	}

	void Display1(void)
	{
		glClear(GL_COLOR_BUFFER_BIT); // 색상 버퍼 초기화
		glColor3f(1.0, 0.0, 0.0);     // 빨간색 설정
		// 좌단 위치 (30, 30) & 우상단 위치 (50, 50)
		glRectf(30.0, 30.0, 50.0, 50.0);
		// 그래픽 카드 메모리에 그린것을 화면에 넣어놓은 Buffer로 덮어 씌워라.
		glutSwapBuffers();
	}

	// /*
	// Culling은 면의 앞/뒤를 판단해서 그릴지 말지 결정하는 것이고,
	// Clipping은 view volume 밖의 geometry를 잘라내는 것입니다.
	// 이번 경우는 티팟이 view volume보다 커서 clipping된 것이지,
	// back face가 보여서 안 그려진 게 아닙니다.
	// */
	// void Display2(void)
	// {
	// 	// // 직교 투영 — 기본 상태와 동일 ([-1,1] 범위)
	// 	glClear(GL_COLOR_BUFFER_BIT);

	// 	// 생략 하면 → 마지막으로 설정된 행렬 모드에 적용됨
	// 	// (기본값은 GL_MODELVIEW). 의도치 않게 modelview 행렬에 투영을 곱하게 될 수 있음
	// 	glMatrixMode(GL_PROJECTION);

	// 	// 생략 하면 → 이전 프레임의 행렬 위에 계속 누적되어 화면이 점점 왜곡됨
	// 	glLoadIdentity();

	// 	// gluPerspective(90.0, 1.0, 0.01, 100.0); // ! 원근 적용
	// 	glOrtho(-1.0, 1.0, -1.0, 1.0, -100.0, 100.0); // ! 오쏘 적용

	// 	// 카메라를 티팟 내부(원점)에 배치, 살짝 방향을 틀어서 내부 면이 보이게
	// 	glMatrixMode(GL_MODELVIEW);
	// 	glLoadIdentity();

	// 	// gl 접두사는 "GL device 객체"가 아니라 OpenGL API 함수라는 의미입니다.
	// 	// glDisable = "OpenGL의 해당 기능을 비활성화하라"입니다.
	// 	glDisable(GL_CULL_FACE);

	// 	glColor3f(0.0, 1.0, 1.0); // 시안(cyan)색 설정
	// 	glutSolidTeapot(2.5);	  // 솔리드 티팟 렌더링 (크기 2.5)
	// 	glColor3f(1.0, 0.0, 0.0); // 빨간색 설정
	// 	glutWireTeapot(2.5);	  // 와이어프레임 티팟 렌더링 (크기 2.5)

	// 	glFlush(); // GPU VRAM에 넘겨줘라 (Call) 하라는 의미
	// }

	// void Display3()
	// {
	// 	GLfloat size[2];	      // 포인트 크기 범위 저장용
	// 	float angle;		      // 원 위의 각도
	// 	glClear(GL_COLOR_BUFFER_BIT); // 색상 버퍼 초기화
	// 	glColor3f(1.0, 1.0, 1.0);     // 흰색 설정

	// 	// 현재 OpenGL 구현이 지원하는 점(point) 크기의 최소/최대 범위를 조회하는 함수입니다.
	// 	// 	* glGetFloatv — OpenGL 상태 값을 float로 가져옴
	// 	// 	* GL_POINT_SIZE_RANGE — 점 크기 범위를 요청
	// 	// 	* size[0] = 최소 크기, size[1] = 최대 크기
	// 	glGetFloatv(GL_POINT_SIZE_RANGE, size);

	// 	// glPointSize(size[0])에서 최소 크기로 점을 설정하고 있습니다.
	// 	//	* 하드웨어마다 지원하는 범위가 다르기 때문에,
	// 	//	* 하드코딩 대신 조회해서 쓰는 방식입니다.
	// 	// size[0]은 가장 작은 값, size[1]은 가장 큰 값
	// 	// 	* macOs에서는 1, 64
	// 	std::cout << size[0] << " " << size[1] << std::endl; // 최소/최대 크기 출력
	// 	glPointSize(size[1]);				     // 포인트 크기를 최대값으로 설정

	// 	glBegin(GL_POINTS); // 점 그리기 시작
	// 	// glBegin(GL_LINE_STRIP);
	// 	for (angle = 0; angle <= 2.0f * 3.14f; angle += (2.0f * 3.14f) / 30.f) // 30개의 점으로 원 그리기
	// 		glVertex3f(cos(angle) * 0.5f, sin(angle) * 0.5f, 0);	   // 반지름 0.5인 원 위의 점
	// 	glEnd();							   // 점 그리기 종료
	// 	glFlush();							   // 렌더링 명령 즉시 실행
	// }

	// void Display4()
	// {
	// 	GLfloat size[2] = {
	// 	    // 라인 굵기 범위 저장용
	// 	    0,
	// 	};
	// 	glClear(GL_COLOR_BUFFER_BIT); // 색상 버퍼 초기화
	// 	// ✅ glGetFloatv, glLineWidth는 glBegin/glEnd 밖에서 호출해야 함 ✅
	// 	glGetFloatv(GL_LINE_WIDTH_RANGE, size); // 라인 굵기 최소/최대 범위 조회
	// 	std::cout << "size[0] : " << size[0] << '\n'
	// 		  << "size[1] : " << size[1] << std::endl; // 최소/최대 굵기 출력
	// 	glLineWidth(size[1]);				   // 라인 굵기를 최대값으로 설정

	// 	glBegin(GL_LINES); // 선 그리기 시작
	// 	{
	// 		// ! ❌ 안됨 std::cout << "size[0] : " << size[0] << '\n'
	// 		// ! ❌ 안됨 	  << "size[1] : " << size[1]
	// 		// ! ❌ 안됨 	  << std::endl;
	// 		// ! ❌ 안됨 glGetFloatv(GL_LINE_WIDTH_RANGE, size);
	// 		// ! ❌ 안됨 glLineWidth(size[1]);
	// 		glColor4f(0.0f, 1.0f, 0.0f, 1.0f); // 초록색 (시작점)
	// 		glVertex3f(-1.0f, 0.0f, 0.0f);	   // 선의 시작점 (왼쪽 끝)
	// 		glColor4f(1.0f, 0.0f, 0.0f, 1.0f); // 빨간색 (끝점)
	// 		glVertex3f(1.0f, 0.0f, 0.0f);	   // 선의 끝점 (오른쪽 끝)
	// 	}
	// 	glEnd();   // 선 그리기 종료
	// 	glFlush(); // 렌더링 명령 즉시 실행
	// }

	// void Display5()
	// {
	// 	glClear(GL_COLOR_BUFFER_BIT); // 색상 버퍼 초기화
	// 	glShadeModel(GL_SMOOTH);      // 부드러운 색상 보간 (그라디언트)
	// 	// glShadeModel(GL_FLAT); // 가장 마지막에 넣은 색을 대표 색으로 한다.
	// 	glBegin(GL_TRIANGLES); // 삼각형 그리기 시작
	// 	{
	// 		glColor3f(1.0, 0.0, 0.0);    // 빨간색 (꼭짓점 1)
	// 		glVertex3f(0.0, 0.5f, 0.0);  // 상단 꼭짓점
	// 		glColor3f(0.0, 1.0, 0.0);    // 초록색 (꼭짓점 2)
	// 		glVertex3f(-0.5f, -0.5f, 0); // 좌하단 꼭짓점
	// 		glColor3f(0.0, 0.0, 1.0);    // 파란색 (꼭짓점 3)
	// 		glVertex3f(0.5f, -0.5f, 0);  // 우하단 꼭짓점
	// 	}
	// 	glEnd();   // 삼각형 그리기 종료
	// 	glFlush(); // 렌더링 명령 즉시 실행
	// }
} // namespace SJH::Chapter1