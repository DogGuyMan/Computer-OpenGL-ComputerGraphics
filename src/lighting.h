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
	class Lighting
	{
	  public:
		void Init() const;
		void ApplySunLight() const;
		void ApplyPointLight(float elapsedSeconds) const;
	};
} // namespace Metahuman

#endif // __METAHUMAN_LIGHTING_H__
