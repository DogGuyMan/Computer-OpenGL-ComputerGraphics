#ifndef __METAHUMAN_MODEL_H__
#define __METAHUMAN_MODEL_H__
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

namespace Metahuman {
	class Model {
	public :
		Model();
		~Model();
		void Draw();
	};
};

#endif//__METAHUMAN_MODEL_H__
