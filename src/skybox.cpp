#include "skybox.h"

namespace Metahuman
{
	void Skybox::SetTextures(GLuint px, GLuint nx, GLuint py, GLuint ny, GLuint pz, GLuint nz)
	{
		texPX = px;
		texNX = nx;
		texPY = py;
		texNY = ny;
		texPZ = pz;
		texNZ = nz;
	}

	void Skybox::Draw() const
	{
		glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);   // 조명 영향을 받지 않는 자체 발광 배경
		glEnable(GL_TEXTURE_2D);

		// 이전 프레임의 색상 오염 방지 (순수 텍스처 색상 출력)
		glColor3f(1.0f, 1.0f, 1.0f);

		glPushMatrix();

		float size = 15.0f;
		float bottomY = -10.0f; // 바닥을 좀 더 깊게
		float topY = 15.0f;

		// 1. 바닥 (Bottom: -Y)
		if (texNY != 0) {
			glBindTexture(GL_TEXTURE_2D, texNY);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, bottomY, -size);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(size, bottomY, -size);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(size, bottomY, size);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, bottomY, size);
			glEnd();
		}

		// 2. 뒤쪽 벽 (Back: -Z)
		if (texNZ != 0) {
			glBindTexture(GL_TEXTURE_2D, texNZ);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, bottomY, -size);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(size, bottomY, -size);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(size, topY, -size);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, topY, -size);
			glEnd();
		}

		// 3. 앞쪽 벽 (Front: +Z)
		if (texPZ != 0) {
			glBindTexture(GL_TEXTURE_2D, texPZ);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(size, bottomY, size);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, bottomY, size);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-size, topY, size);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(size, topY, size);
			glEnd();
		}

		// 4. 왼쪽 벽 (Left: -X)
		if (texNX != 0) {
			glBindTexture(GL_TEXTURE_2D, texNX);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, bottomY, size);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, bottomY, -size);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(-size, topY, -size);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, topY, size);
			glEnd();
		}

		// 5. 오른쪽 벽 (Right: +X)
		if (texPX != 0) {
			glBindTexture(GL_TEXTURE_2D, texPX);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(size, bottomY, -size);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(size, bottomY, size);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(size, topY, size);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(size, topY, -size);
			glEnd();
		}

		// 6. 천장 (Top: +Y)
		if (texPY != 0) {
			glBindTexture(GL_TEXTURE_2D, texPY);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, topY, -size);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(size, topY, -size);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(size, topY, size);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, topY, size);
			glEnd();
		}

		glPopMatrix();

		// 이전 상태 복구
		glPopAttrib();
	}
} // namespace Metahuman