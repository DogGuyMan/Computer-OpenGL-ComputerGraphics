#ifndef __METAHUMAN_DISPLAY_H__
#define __METAHUMAN_DISPLAY_H__
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif
#include "camera.h"
#include "model.h"
#include <vector>
#include <memory>

namespace Metahuman {
	class Display {
	private :
		int width;
		int height;
		std::vector<std::unique_ptr<Model>> models;
	public :
		Display();
		~Display();
		void Reshape(int w, int h, Camera& camera);
		void Render(Camera& camera);
		void AddModel(std::unique_ptr<Model> model);
		
		int GetWidth() const;
		int Getheight() const;
		double GetAspectRatio() const;

		void SetWidth(int w);
		void Setheight(int h);
	};
};

#endif//__METAHUMAN_DISPLAY_H__
