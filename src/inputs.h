#ifndef __METAHUMAN_INPUTS_H__
#define __METAHUMAN_INPUTS_H__
#include <functional>

namespace Metahuman {
	class KeybaordInput {
	private :
		std::function<void()> keyBindedActions[256];
	public :
		KeybaordInput();
		~KeybaordInput();
		void BindKeyAction(unsigned char key, std::function<void()> action);
		void UnbindKeyAction(unsigned char key);
		void Dispatch(unsigned char key);
	};

	class MouseInput {
	private :
		bool isDragging;
		int lastX;
		int lastY;
		// 드래그 시 deltaX, deltaY를 전달
		std::function<void(int, int)> dragAction;
	public :
		MouseInput();
		~MouseInput();
		void BindDragAction(std::function<void(int, int)> action);
		// GLUT 콜백에서 호출
		void HandleMouse(int button, int state, int x, int y);
		void HandleMotion(int x, int y);
	};
};

#endif//__METAHUMAN_INPUTS_H__
