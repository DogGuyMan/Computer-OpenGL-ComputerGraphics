#include "display.h"
#include <cmath>
#include <spdlog/spdlog.h>

#define RAMP_SIZE 256

using namespace Metahuman;

Display::Display() : width(800), height(600)
{
}

Display::~Display() {

}


void Display::Reshape(int w, int h, Camera& camera)
{
	if(h == 0) h = 1;
	width = w;
	height = h;
	glViewport(0, 0, width, height);

	// 투영 행렬은 Camera 책임
	camera.ApplyProjection((float)GetAspectRatio());
	glMatrixMode(GL_MODELVIEW);

	spdlog::info("Metahuman::Display::Reshape\nCurrent Width and Height : {0}, {1}", width, height);
}


int Display::GetWidth() const {
	return width;
}
int Display::Getheight() const {
	return height;
}

double Display::GetAspectRatio() const {
	return (double) width / height;
}

void Display::SetWidth(int w) {
	width = w;
}
void Display::Setheight(int h) {
	height = h;
}
