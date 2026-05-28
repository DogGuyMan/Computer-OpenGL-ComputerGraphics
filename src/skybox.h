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
		float sz;
		GLuint texPX, texNX, texPY, texNY, texPZ, texNZ;

	  public:
		Skybox(float size = 10.0f) // 카메라의 Far Plane 밖으로 벗어나 잘리는(Clipping) 현상 방지
		    : sz(size), texPX(0), texNX(0), texPY(0), texNY(0), texPZ(0), texNZ(0)
		{
		}

		// 스카이박스의 각 면에 해당하는 텍스처 ID 세팅 (순서: +X, -X, +Y, -Y, +Z, -Z)
		void SetTextures(GLuint px, GLuint nx, GLuint py, GLuint ny, GLuint pz, GLuint nz)
		{
			texPX = px;
			texNX = nx;
			texPY = py;
			texNY = ny;
			texPZ = pz;
			texNZ = nz;
		}

		void Draw() const
		{
			// 이전 렌더링 상태 저장
			glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_CURRENT_BIT);

			glDisable(GL_DEPTH_TEST); // 깊이 테스트를 꺼서 무조건 가장 뒤(배경)에 그려지게 함
			glDisable(GL_LIGHTING);   // 스카이박스는 자체 발광하는 배경이므로 조명 무시
			glDisable(GL_BLEND);
			glDisable(GL_CULL_FACE);  // 카메라가 박스 내부에 있을 때 뒷면이 컬링(Culling)되는 현상 방지
			glEnable(GL_TEXTURE_2D);

			// 이전 렌더링(UI 등)의 색상 상태 오염을 막기 위해 강제 흰색 적용
			glColor3f(1.0f, 1.0f, 1.0f); 

			glPushMatrix();

			// 카메라의 위치 이동(Translation) 무시 및 제자리에 스카이박스 고정
			// (ModelView 행렬의 Translation 요소를 0으로 만듦)
			GLfloat matrix[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
			matrix[12] = 0.0f;
			matrix[13] = 0.0f;
			matrix[14] = 0.0f;
			glLoadMatrixf(matrix);

			float d = sz / 2.0f;

			// 1. PX (Right: +X)
			glBindTexture(GL_TEXTURE_2D, texPX);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(d, -d, d);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(d, -d, -d);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(d, d, -d);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(d, d, d);
			glEnd();

			// 2. NX (Left: -X)
			glBindTexture(GL_TEXTURE_2D, texNX);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-d, -d, -d);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-d, -d, d);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(-d, d, d);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-d, d, -d);
			glEnd();

			// 3. PY (Top: +Y)
			glBindTexture(GL_TEXTURE_2D, texPY);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-d, d, d);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(d, d, d);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(d, d, -d);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-d, d, -d);
			glEnd();

			// 4. NY (Bottom: -Y)
			glBindTexture(GL_TEXTURE_2D, texNY);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-d, -d, -d);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(d, -d, -d);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(d, -d, d);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-d, -d, d);
			glEnd();

			// 5. PZ (Front: +Z)
			glBindTexture(GL_TEXTURE_2D, texPZ);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-d, -d, d);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(d, -d, d);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(d, d, d);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-d, d, d);
			glEnd();

			// 6. NZ (Back: -Z)
			glBindTexture(GL_TEXTURE_2D, texNZ);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(d, -d, -d);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-d, -d, -d);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(-d, d, -d);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(d, d, -d);
			glEnd();

			glPopMatrix();

			// 이전 상태 복구
			glPopAttrib();
		}
	};
} // namespace Metahuman

#endif // __METAHUMAN_SKYBOX_H__