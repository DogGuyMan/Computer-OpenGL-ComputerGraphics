#ifndef __METAHUMAN_DISPLAY_H__
#define __METAHUMAN_DISPLAY_H__
#include "camera.h"

namespace Metahuman {
	class Display {
	private :	
		Camera camera;
		static void HandleWindowReshapeEvent(int w, int h);
		static void HandleDisplayEvent(void);
	public :
		Display();	
		~Display();
	};
};

#endif//__METAHUMAN_DISPLAY_H__