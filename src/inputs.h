#ifndef __METAHUMAN_INPUTS_H__
#define __METAHUMAN_INPUTS_H__

namespace Metahuman {
	class KeybaordInput {
	private : 
		static void (*keyBindedActions[256])(void* datas);
		static void handleKeyboardInput(unsigned char key, int x, int y);
	public :
		KeybaordInput();
		~KeybaordInput();
		void BindKeyAction(unsigned char key, void (*action)(void* datas));
		void UnbindKeyAction(unsigned char key);
	};
};

#endif//__METAHUMAN_INPUTS_H__