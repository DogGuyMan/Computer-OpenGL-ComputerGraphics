#include "inputs.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

using namespace Metahuman;

KeybaordInput::KeybaordInput() : keyBindedActions{} {
}

KeybaordInput::~KeybaordInput() {
	for(unsigned int i = 0; i < 256; i++)
		UnbindKeyAction((unsigned char)i);
}

void KeybaordInput::BindKeyAction(unsigned char key, std::function<void()> action) {
	keyBindedActions[key] = action;
}

void KeybaordInput::UnbindKeyAction(unsigned char key) {
	keyBindedActions[key] = nullptr;
}

void KeybaordInput::Dispatch(unsigned char key) {
	if(keyBindedActions[key])
		keyBindedActions[key]();
}

MouseInput::MouseInput() : isDragging(false), lastX(0), lastY(0) {
}

MouseInput::~MouseInput() {
}

void MouseInput::BindDragAction(std::function<void(int, int)> action) {
	dragAction = action;
}

void MouseInput::HandleMouse(int button, int state, int x, int y) {
	if(button == GLUT_LEFT_BUTTON) {
		if(state == GLUT_DOWN) {
			isDragging = true;
			lastX = x;
			lastY = y;
		} else {
			isDragging = false;
		}
	}
}

void MouseInput::HandleMotion(int x, int y) {
	if(isDragging && dragAction) {
		int deltaX = x - lastX;
		int deltaY = y - lastY;
		lastX = x;
		lastY = y;
		dragAction(deltaX, deltaY);
		glutPostRedisplay();
	}
}
