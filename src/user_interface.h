#ifndef __METAHUMAN_USER_INTERFACE_H__
#define __METAHUMAN_USER_INTERFACE_H__
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
	void DummyTransform();
	void UIUpdate(float* pos);
	void UIReshape(int w, int h);
	void UIMouse(int button, int state, int x, int y);
	void UIMotion(int x, int y);
	void UIKeyboardInput(unsigned char key, int x, int y);
}

#endif//__METAHUMAN_USER_INTERFACE_H__