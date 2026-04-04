#include "chapter2.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h.h>
#endif
#include <iostream>
#include <spdlog/spdlog.h>

void Tutorials::Chapter2::Callbacks::HandleWindowReshapeEvent(int w, int h)
{
	float aspectRatio = (float)w / (float)h;
	// 이전 프레임의 변환 누적을 방지
	glLoadIdentity();
	// 윈도우 크기 변경 시 렌더링 영역 동기화 필수
	glViewport(0, 0, w, h);

	// 생략 하면 -> 마지막으로 설정된 행렬 모드에 적용됨
	// (기본값은 GL_MODELVIEW). 의도치 않게 modelview 행렬에 투영을 곱하게 될 수 있음
	glMatrixMode(GL_PROJECTION);
    	gluPerspective(45.0,                  // FOV
                   aspectRatio,   // 종횡비
                   0.1,                   // 근거리 클립
                   100.0);                // 원거리 클립

	spdlog::info("Tutorials::Chapter2::Callbacks::HandleWindowReshapeEvent\nCurrent Width and Height : {0}, {1}", w, h);
}

/*
Culling은 면의 앞/뒤를 판단해서 그릴지 말지 결정하는 것이고,
Clipping은 view volume 밖의 geometry를 잘라내는 것입니다.
이번 경우는 티팟이 view volume보다 커서 clipping된 것이지,
back face가 보여서 안 그려진 게 아닙니다.
*/
void Tutorials::Chapter2::Callbacks::HandleDisplayEvent(void)
{
	// 직교 투영 — 기본 상태와 동일 ([-1,1] 범위)
	glClear(GL_COLOR_BUFFER_BIT);


	// 생략 하면 -> 이전 프레임의 행렬 위에 계속 누적되어 화면이 점점 왜곡됨
	glLoadIdentity();

	gluPerspective(90.0, 1.0, 0.01, 100.0); // ! 원근 적용

	// 카메라를 티팟 내부(원점)에 배치, 살짝 방향을 틀어서 내부 면이 보이게
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// gl 접두사는 "GL device 객체"가 아니라 OpenGL API 함수라는 의미입니다.
	// glDisable = "OpenGL의 해당 기능을 비활성화하라"입니다.
	glDisable(GL_CULL_FACE);

	glColor3f(0.0, 1.0, 1.0); // 시안(cyan)색 설정
	glutSolidTeapot(2.5);	  // 솔리드 티팟 렌더링 (크기 2.5)
	glColor3f(1.0, 0.0, 0.0); // 빨간색 설정
	glutWireTeapot(2.5);	  // 와이어프레임 티팟 렌더링 (크기 2.5)

	glFlush(); // GPU VRAM에 넘겨줘라 (Call) 하라는 의미
}

void Tutorials::Chapter2::Callbacks::HandleInputKeyboardEvent(void)
{
}