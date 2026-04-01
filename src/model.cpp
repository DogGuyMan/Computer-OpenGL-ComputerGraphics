#include "model.h"

using namespace Metahuman;

Model::Model() {
}

Model::~Model() {
}

void Model::Draw() {
	glColor3f(1.0f, 0.0f, 0.0f);
	glutSolidSphere(1, 20, 20);
}
