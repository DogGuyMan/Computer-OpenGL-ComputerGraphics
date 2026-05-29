#ifndef __METAHUMAN_LIGHTING_H__
#define __METAHUMAN_LIGHTING_H__

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

// OpenGL 1.2 separate specular color control.
// Windows 기본 gl.h는 OpenGL 1.1까지만 보장하므로 누락된 상수를 보강한다.
#ifndef GL_LIGHT_MODEL_COLOR_CONTROL
#define GL_LIGHT_MODEL_COLOR_CONTROL 0x81F8
#endif

#ifndef GL_SEPARATE_SPECULAR_COLOR
#define GL_SEPARATE_SPECULAR_COLOR 0x81FA
#endif

namespace Metahuman
{
	struct LightingValue
	{
		float sunThetaDeg = 60.0f;
		float sunPhiDeg = 128.0f;
		float sunAmbient[3] = {0.22f, 0.22f, 0.22f};
		float sunDiffuse[3] = {0.88f, 0.86f, 0.80f};
		float sunSpecular[3] = {1.0f, 1.0f, 1.0f};
		float materialSpecular[3] = {1.0f, 1.0f, 1.0f};
		float materialShininess = 64.0f;
		float pointDiffuse[3] = {1.0f, 0.0f, 0.0f};
		float pointSpecular[3] = {1.0f, 0.0f, 0.0f};
		float pointRadius = 3.0f;
		float pointHeight = 0.8f;
		float pointAngularSpeed = 1.5f;
	};

	class Lighting
	{
	  private:
		LightingValue value;

	  public:
		void Init() const;
		void ApplySunLight() const;
		void ApplyPointLight(float elapsedSeconds) const;
		LightingValue &GetValue();
	};
} // namespace Metahuman

#endif // __METAHUMAN_LIGHTING_H__
