#include "lighting.h"

namespace Metahuman
{
	void Lighting::Init() const
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_NORMALIZE);
		glEnable(GL_COLOR_MATERIAL);
		glShadeModel(GL_SMOOTH);

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		const GLfloat ambient[] = {0.22f, 0.22f, 0.22f, 1.0f};
		const GLfloat diffuse[] = {0.88f, 0.86f, 0.80f, 1.0f};
		const GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
		const GLfloat materialSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
		const GLfloat materialShininess[] = {64.0f};

		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);
	}

	void Lighting::ApplySunLight() const
	{
		const GLfloat sunDirection[] = {-0.35f, 1.0f, 0.45f, 0.0f};
		glLightfv(GL_LIGHT0, GL_POSITION, sunDirection);
	}
} // namespace Metahuman
