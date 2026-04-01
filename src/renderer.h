#ifndef __METAHUMAN_RENDERER_H__
#define __METAHUMAN_RENDERER_H__
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include <vector>
#include "camera.h"
#include "model.h"

namespace Metahuman {
	class Renderer {
	private :
		std::vector<std::unique_ptr<Model>> models;
	public :
		Renderer();
		~Renderer();
		void Render(Camera& camera);
		void AddModel(std::unique_ptr<Model> model);
	};
};

#endif//__METAHUMAN_RENDERER_H__
