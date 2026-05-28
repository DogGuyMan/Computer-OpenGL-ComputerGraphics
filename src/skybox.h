#ifndef __METAHUMAN_SKYBOX_H__
#define __METAHUMAN_SKYBOX_H__

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

namespace Metahuman
{
	class Skybox
	{
	  private:
		GLuint texPX, texNX, texPY, texNY, texPZ, texNZ;

	  public:
		Skybox(float size = 10.0f) // 카메라의 Far Plane 밖으로 벗어나 잘리는(Clipping) 현상 방지
		    : texPX(0), texNX(0), texPY(0), texNY(0), texPZ(0), texNZ(0)
		{
		}

		// 스카이박스의 각 면에 해당하는 텍스처 ID 세팅 (순서: +X, -X, +Y, -Y, +Z, -Z)
		void SetTextures(GLuint px, GLuint nx, GLuint py, GLuint ny, GLuint pz, GLuint nz);

		void Draw() const;
	};
} // namespace Metahuman

#endif // __METAHUMAN_SKYBOX_H__