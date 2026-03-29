#include "inputs.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

using namespace Metahuman;

KeybaordInput::KeybaordInput() {
	glutKeyboardFunc(handleKeyboardInput);
}

KeybaordInput::~KeybaordInput() {
	for(unsigned int i = 0; i < 256; i++) 
		UnbindKeyAction((unsigned char)i);
}

inline void KeybaordInput::BindKeyAction(unsigned char key, void (*action)(void* datas)) 
{
	keyBindedActions[key] = action;
}

inline void KeybaordInput::UnbindKeyAction(unsigned char key) 
{
	keyBindedActions[key] = nullptr;
}

void KeybaordInput::handleKeyboardInput(unsigned char key, int x, int y) {
	if(!keyBindedActions[key])
		keyBindedActions[key](nullptr);
	glutPostRedisplay();
}