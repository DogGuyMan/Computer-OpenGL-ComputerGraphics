#ifndef __METAHUMAN_GROUND_H__
#define __METAHUMAN_GROUND_H__

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

namespace Metahuman
{
	class Ground
	{
	  private:
		GLuint textureID = 0;
		float y = -2.33f;
		float size = 8.0f;

	  public:
		void SetTexture(GLuint texture);
		void Draw() const;
		void SetY(float newy)  {
			y = newy;
		}
	};
} // namespace Metahuman

#endif // __METAHUMAN_GROUND_H__
