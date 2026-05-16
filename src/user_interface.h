#ifndef __METAHUMAN_USER_INTERFACE_H__
#define __METAHUMAN_USER_INTERFACE_H__
#include "transformable.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif
#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl2.h"

#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed
#endif

namespace Metahuman {
	void InitImgui();

	// 한 프레임 단위 UI begin/end. 사이에 패널들을 호출한다.
	// 패널을 2개 이상 그릴 때 NewFrame이 한 프레임에 두 번 불리지 않도록 분리됨.
	void UIBeginFrame();
	void UIEndFrame();

	// 패널들 (UIBeginFrame ~ UIEndFrame 사이에서 호출)
	// modelIndex/modelCount: Transform 패널 상단 Combo로 조작 대상 모델을 선택. modelIndex는 in/out.
	void UITransformPanel(const char* label, Metahuman::PODTransform& form,
	                      int& modelIndex, int modelCount);
	void UIUVPanel(const char* label, Metahuman::UVTransform& uv);

	void UIReshape(int w, int h);
	void UIMouse(int button, int state, int x, int y);
	void UIMouseWheel(int button, int dir, int x, int y);
	void UIMotion(int x, int y);
	void UIKeyboardInput(unsigned char key, int x, int y);
	void UIKeyboardUpInput(unsigned char key, int x, int y);
	void UISpecialInput(int key, int x, int y);
	void UISpecialUpInput(int key, int x, int y);
}

#endif//__METAHUMAN_USER_INTERFACE_H__
