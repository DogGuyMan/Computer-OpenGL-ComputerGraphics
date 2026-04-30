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

void Model::Translate(const glm::fvec3 &pos) {
	modelTMatrix = glm::translate(glm::mat4(1.0f), pos);
	dirtyFlag = true;
}

void Model::Rotate(const glm::fvec3 &eulerDeg) {
	const glm::vec3 r = glm::radians(eulerDeg);
	const glm::mat4 rx = glm::rotate(glm::mat4(1.0f), r.x, glm::vec3(1.0f, 0.0f, 0.0f));
	const glm::mat4 ry = glm::rotate(glm::mat4(1.0f), r.y, glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 rz = glm::rotate(glm::mat4(1.0f), r.z, glm::vec3(0.0f, 0.0f, 1.0f));
	modelRMatrix = rx * ry * rz;
	dirtyFlag = true;
}

void Model::Scale(const glm::fvec3 &factor) {
	modelSMatrix = glm::scale(glm::mat4(1.0f), factor);
	dirtyFlag = true;
}

void Model::SetTransform(const Metahuman::PODTransform& t) {
	Translate(t.translate);
	Rotate(t.eulerDeg);
	Scale(t.scale);
}