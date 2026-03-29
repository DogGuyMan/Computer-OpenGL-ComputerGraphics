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

void Display::Render(Camera& camera)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 매 프레임 투영 갱신 (FOV 변경 반영)
	camera.ApplyProjection((float)GetAspectRatio());
	camera.ApplyView();

	// 등록된 모델 순회 렌더링
	for(auto& model : models)
		model->Draw();

	glutSwapBuffers();
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

void Display::AddModel(std::unique_ptr<Model> model) {
	models.push_back(std::move(model));
}

void Display::SetWidth(int w) {
	width = w;
}
void Display::Setheight(int h) {
	height = h;
}
