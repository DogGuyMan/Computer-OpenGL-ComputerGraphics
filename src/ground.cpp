#include "ground.h"

namespace Metahuman
{
	void Ground::SetTexture(GLuint texture)
	{
		textureID = texture;
	}

	void Ground::Draw() const
	{
		if (textureID == 0)
			return;

		glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_CURRENT_BIT);

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glColor3f(1.0f, 1.0f, 1.0f);

		const float repeat = 4.0f;

		glBegin(GL_QUADS);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-size, y, -size);
		glTexCoord2f(repeat, 0.0f);
		glVertex3f(size, y, -size);
		glTexCoord2f(repeat, repeat);
		glVertex3f(size, y, size);
		glTexCoord2f(0.0f, repeat);
		glVertex3f(-size, y, size);
		glEnd();

		glPopAttrib();
	}
} // namespace Metahuman
