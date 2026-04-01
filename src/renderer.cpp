#include "renderer.h"

using namespace Metahuman;

Renderer::Renderer() {

}

Renderer::~Renderer() {
	for(auto& model : models)
		model.reset();
}

void Renderer::Render(Camera& camera)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 매 프레임 투영 갱신 (FOV 변경 반영)
	camera.ApplyProjection((float)GetAspectRatio());
	camera.ApplyView();

	// 등록된 모델 순회 렌더링
	for(auto& model : models)
		model->Draw();

}

void Renderer::AddModel(std::unique_ptr<Model> model) {
	models.push_back(std::move(model));
}