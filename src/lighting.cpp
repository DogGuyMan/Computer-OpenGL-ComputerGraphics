#include "lighting.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Metahuman
{
	namespace
	{
		float WrapDegrees(float degrees)
		{
			const float wrapped = std::fmod(degrees, 360.0f);
			return wrapped < 0.0f ? wrapped + 360.0f : wrapped;
		}
	} // namespace

	void Lighting::Init() const
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_NORMALIZE);
		glEnable(GL_COLOR_MATERIAL);
		glShadeModel(GL_SMOOTH);

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		const GLfloat ambient[] = {value.sunAmbient[0], value.sunAmbient[1], value.sunAmbient[2], 1.0f};
		const GLfloat diffuse[] = {value.sunDiffuse[0], value.sunDiffuse[1], value.sunDiffuse[2], 1.0f};
		const GLfloat specular[] = {value.sunSpecular[0], value.sunSpecular[1], value.sunSpecular[2], 1.0f};
		const GLfloat materialSpecular[] = {value.materialSpecular[0], value.materialSpecular[1], value.materialSpecular[2], 1.0f};
		const GLfloat materialShininess[] = {value.materialShininess};
		const GLfloat pointAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		const GLfloat pointDiffuse[] = {value.pointDiffuse[0], value.pointDiffuse[1], value.pointDiffuse[2], 1.0f};
		const GLfloat pointSpecular[] = {value.pointSpecular[0], value.pointSpecular[1], value.pointSpecular[2], 1.0f};

		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

		glLightfv(GL_LIGHT1, GL_AMBIENT, pointAmbient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, pointDiffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, pointSpecular);
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.05f);
		glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.01f);

		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);
	}

	void Lighting::ApplySunLight() const
	{
		const float theta = WrapDegrees(value.sunThetaDeg) * (float)M_PI / 180.0f;
		const float phi = WrapDegrees(value.sunPhiDeg) * (float)M_PI / 180.0f;
		const float horizontal = std::cos(theta);
		const GLfloat ambient[] = {value.sunAmbient[0], value.sunAmbient[1], value.sunAmbient[2], 1.0f};
		const GLfloat diffuse[] = {value.sunDiffuse[0], value.sunDiffuse[1], value.sunDiffuse[2], 1.0f};
		const GLfloat specular[] = {value.sunSpecular[0], value.sunSpecular[1], value.sunSpecular[2], 1.0f};
		const GLfloat materialSpecular[] = {value.materialSpecular[0], value.materialSpecular[1], value.materialSpecular[2], 1.0f};
		const GLfloat materialShininess[] = {value.materialShininess};
		const GLfloat sunDirection[] = {
		    horizontal * std::cos(phi),
		    std::sin(theta),
		    horizontal * std::sin(phi),
		    0.0f};
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);
		glLightfv(GL_LIGHT0, GL_POSITION, sunDirection);
	}

	void Lighting::ApplyPointLight(float elapsedSeconds) const
	{
		const GLfloat pointDiffuse[] = {value.pointDiffuse[0], value.pointDiffuse[1], value.pointDiffuse[2], 1.0f};
		const GLfloat pointSpecular[] = {value.pointSpecular[0], value.pointSpecular[1], value.pointSpecular[2], 1.0f};
		const float angle = elapsedSeconds * value.pointAngularSpeed;
		const GLfloat pointPosition[] = {
		    std::cos(angle) * value.pointRadius,
		    value.pointHeight,
		    std::sin(angle) * value.pointRadius,
		    1.0f};

		glLightfv(GL_LIGHT1, GL_DIFFUSE, pointDiffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, pointSpecular);
		glLightfv(GL_LIGHT1, GL_POSITION, pointPosition);
	}

	LightingValue &Lighting::GetValue()
	{
		return value;
	}
} // namespace Metahuman
