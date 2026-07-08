#ifndef __METAHUMAN_MATERIAL_H__
#define __METAHUMAN_MATERIAL_H__
#include "resource_management.h"
#include "transformable.h"
#include <glm/glm.hpp>

namespace Metahuman
{

	class ITechnique;

	struct Material
	{
		Texture *albedoPtr = nullptr;
		UVValue uv;
		glm::vec3 baseColor{1, 1, 1};
		ITechnique *techniquePtr = nullptr;
	};

	class ITechnique
	{
	  public:
		virtual ~ITechnique() = default;
		virtual void Bind(const Material &) = 0;
		virtual void UnBind(const Material &) = 0;
	};

	class TextureTechnique : public ITechnique
	{
	public : 
		TextureTechnique() = default;
		void Bind(const Material &mt) override
		{
    			Texture*         texture = mt.albedoPtr;
    			const UVValue&   uv      = mt.uv;
    			const glm::vec3& color   = mt.baseColor;
			const GLuint id = texture ? texture->GetTextureID() : 0;
			if (id != 0)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, id);

				glMatrixMode(GL_TEXTURE);
				glPushMatrix();
				glLoadIdentity();
				glTranslatef(uv.offset.x, uv.offset.y, 0.0f);
				glRotatef(uv.rotationDeg, 0.0f, 0.0f, 1.0f);
				glScalef(uv.scale.x, uv.scale.y, 1.0f);
				glMatrixMode(GL_MODELVIEW);
			}
			glColor3f(color.r, color.g, color.b);
		}
		void UnBind(const Material &mt) override
		{
			Texture*         texture = mt.albedoPtr;
    			const UVValue&   uv      = mt.uv;
    			const glm::vec3& color   = mt.baseColor;
			const GLuint id = texture ? texture->GetTextureID() : 0; // 2. Appearence
			if (id != 0)
			{
				glMatrixMode(GL_TEXTURE);   // ???
				glPopMatrix();              // ???
				glMatrixMode(GL_MODELVIEW); // ???
				glDisable(GL_TEXTURE_2D);   // ???
			}
		}
	};


}; // namespace Metahuman

#endif //__METAHUMAN_MATERIAL_H__
