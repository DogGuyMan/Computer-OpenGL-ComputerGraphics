#include "model.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace Metahuman;

Model::Model() {
}

Model::~Model() {
}

void Model::recalculateModelMatrix() {
	if (!dirtyFlag) return;
	// TRS 순서: Translation * Rotation * Scale
	modelMatrix = modelTMatrix * modelRMatrix * modelSMatrix;
	dirtyFlag = false;
}

const glm::mat4& Model::GetModelMatrix() {
	recalculateModelMatrix();
	return modelMatrix;
}

void Model::Draw() {
	recalculateModelMatrix();

	glPushMatrix();
	glMultMatrixf(glm::value_ptr(modelMatrix));

	// 기본 도형 그리기
	glColor3f(1.0f, 0.0f, 0.0f);
	glutWireSphere(1, 20, 20);

	glPopMatrix();
}

void Model::Translate(const glm::fvec3 &delta) {
	modelTMatrix = glm::translate(modelTMatrix, delta);
	dirtyFlag = true;
}

void Model::Rotate(float angleDeg, const glm::fvec3 &axis) {
	modelRMatrix = glm::rotate(modelRMatrix, glm::radians(angleDeg), axis);
	dirtyFlag = true;
}

void Model::Scale(const glm::fvec3 &factor) {
	modelSMatrix = glm::scale(modelSMatrix, factor);
	dirtyFlag = true;
}
