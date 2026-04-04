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
	// 등록된 모델 순회 렌더링
	for(auto& model : models)
		model->Draw();

}

void Renderer::AddModel(std::unique_ptr<Model> model) {
	models.push_back(std::move(model));
}