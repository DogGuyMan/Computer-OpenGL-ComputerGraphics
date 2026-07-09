#ifndef __METAHUMAN_MATERIAL_H__
#define __METAHUMAN_MATERIAL_H__
#include "resource_management.h"
#include "transformable.h"
#include <glm/glm.hpp>
#include <vector>

namespace Metahuman
{

	static inline void LogFragmentState(const char *tag)
	{
		GLint alphaFunc = 0;
		glGetIntegerv(GL_ALPHA_TEST_FUNC, &alphaFunc);
		GLfloat alphaRef = 0;
		glGetFloatv(GL_ALPHA_TEST_REF, &alphaRef);
		GLint blendSrc = 0, blendDst = 0;
		glGetIntegerv(GL_BLEND_SRC, &blendSrc);
		glGetIntegerv(GL_BLEND_DST, &blendDst);
		GLint depthFunc = 0;
		glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
		GLint cullMode = 0;
		glGetIntegerv(GL_CULL_FACE_MODE, &cullMode);
		GLint texBind = 0;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &texBind);
		GLint matMode = 0;
		glGetIntegerv(GL_MATRIX_MODE, &matMode);
		GLfloat color[4];
		glGetFloatv(GL_CURRENT_COLOR, color);

		printf("[GLState %-16s] ALPHA_TEST=%d (func=0x%04X ref=%.2f) | BLEND=%d (0x%X,0x%X) "
		       "| DEPTH=%d (func=0x%X) | CULL=%d (mode=0x%X) | TEX2D=%d bind=%d | LIGHTING=%d "
		       "| MATMODE=0x%X | color=(%.2f,%.2f,%.2f,%.2f)\n",
		       tag,
		       glIsEnabled(GL_ALPHA_TEST), alphaFunc, alphaRef,
		       glIsEnabled(GL_BLEND), blendSrc, blendDst,
		       glIsEnabled(GL_DEPTH_TEST), depthFunc,
		       glIsEnabled(GL_CULL_FACE), cullMode,
		       glIsEnabled(GL_TEXTURE_2D), texBind,
		       glIsEnabled(GL_LIGHTING),
		       matMode,
		       color[0], color[1], color[2], color[3]);
	}
	class ITechnique;

	struct Material
	{
		Texture *albedoPtr = nullptr;
		UVValue uv;
		glm::vec3 baseColor{1, 1, 1};
		std::vector<ITechnique *> passes;
		// per-model 아웃라인 두께. aggregate 초기화 마지막 자리라 model_imp.h는
		// {tex, uv, color, techniques}만 넘겨도 여기 default(1.03f)가 적용된다.
		float outlineScale = 1.03f;
	};

	class ITechnique
	{
	  public:
		virtual ~ITechnique() = default;
		virtual void Bind(const Material &) = 0;
		virtual void Unbind(const Material &) = 0;
	};

	class TextureTechnique : public ITechnique
	{
	  public:
		TextureTechnique() = default;
		void Bind(const Material &mt) override
		{
			Texture *texture = mt.albedoPtr;
			const UVValue &uv = mt.uv;
			const glm::vec3 &color = mt.baseColor;
			const GLuint id = texture ? texture->GetTextureID() : 0;
			glDisable(GL_LIGHTING);
			if (id != 0)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, id);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

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

		void Unbind(const Material &mt) override
		{
			glEnable(GL_LIGHTING);
			Texture *texture = mt.albedoPtr;
			const UVValue &uv = mt.uv;
			const glm::vec3 &color = mt.baseColor;
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

	class AlphaTestTechnique : public TextureTechnique
	{
	  public:
		AlphaTestTechnique() = default;
		void Bind(const Material &mt) override
		{
			// LogFragmentState("BindEnter");
			TextureTechnique::Bind(mt);
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.9f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 여기로
			                                                   // LogFragmentState("BindExit");
		}

		void Unbind(const Material &mt) override
		{
			// LogFragmentState("UnbindEnter");
			glDisable(GL_BLEND);
			glDisable(GL_ALPHA_TEST);
			TextureTechnique::Unbind(mt);
			// LogFragmentState("UnbindExit");
		}
	};

	class CartoonTechnique : public ITechnique
	{
	  private:
		const glm::vec3 *const mLightDirEyePtr;
		GLuint mRampTex = 0;

#define RAMP_SIZE 256

		void buildRamp(unsigned char *pixels, int style)
		{
			int i;
			for (i = 0; i < RAMP_SIZE; i++)
			{
				float t = (float)i / (float)(RAMP_SIZE - 1); /* 0..1 */
				unsigned char v;

				switch (style)
				{
				case 0: /* 2-tone: hard shadow/lit split */
					v = (t < 0.45f) ? 80 : 230;
					break;

				case 1: /* 3-tone: shadow / mid / highlight */
					if (t < 0.30f)
						v = 60;
					else if (t < 0.60f)
						v = 155;
					else
						v = 235;
					break;

				case 2: /* smooth-step bands (softer cartoon feel) */
				{
					/* 3 smooth-step transitions */
					float s = 0.0f;
					/* band 1 */
					{
						float e0 = 0.15f, e1 = 0.35f, x = (t - e0) / (e1 - e0);
						if (x < 0)
							x = 0;
						if (x > 1)
							x = 1;
						s += x * x * (3 - 2 * x) * 0.35f;
					}
					/* band 2 */
					{
						float e0 = 0.40f, e1 = 0.60f, x = (t - e0) / (e1 - e0);
						if (x < 0)
							x = 0;
						if (x > 1)
							x = 1;
						s += x * x * (3 - 2 * x) * 0.35f;
					}
					/* band 3 */
					{
						float e0 = 0.65f, e1 = 0.80f, x = (t - e0) / (e1 - e0);
						if (x < 0)
							x = 0;
						if (x > 1)
							x = 1;
						s += x * x * (3 - 2 * x) * 0.30f;
					}
					v = (unsigned char)(s * 255.0f);
					break;
				}
				default:
					v = (unsigned char)(t * 255.0f);
					break;
				}
				/* RGB = uniform gray-ish tint; we modulate with material color later */
				pixels[i * 3 + 0] = v;
				pixels[i * 3 + 1] = v;
				pixels[i * 3 + 2] = v;
			}
		}

		void uploadRamp(int style)
		{
			unsigned char pixels[RAMP_SIZE * 3];
			buildRamp(pixels, style);

			if (!mRampTex)
				glGenTextures(1, &mRampTex);

			glBindTexture(GL_TEXTURE_1D, mRampTex);
			glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB8, RAMP_SIZE, 0,
			             GL_RGB, GL_UNSIGNED_BYTE, pixels);
			/* NEAREST = hard band edges; LINEAR would blur them */
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		}

#undef RAMP_SIZE

	  public:
		explicit CartoonTechnique(const glm::vec3 *lightDirEye)
		    : mLightDirEyePtr(lightDirEye)
		{
			uploadRamp(0);
		}

		~CartoonTechnique() override {
			if(mRampTex){
				glDeleteTextures(1, &mRampTex);
				mRampTex = 0;
			}
		}

		void Bind(const Material &mt) override
		{
			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D); // 1D가 이기도록
			glEnable(GL_TEXTURE_1D);
			glBindTexture(GL_TEXTURE_1D, mRampTex);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			// 좌표 생성 "모드"를 NORMAL_MAP으로 지정 (S/T/R 각각)
			// 	이것만으로는 아무 일도 안 일어남.
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP); // "켜지면 어떻게 좌표를 만들지" 규칙만 등록.
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP); // "켜지면 어떻게 좌표를 만들지" 규칙만 등록.
			glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP); // "켜지면 어떻게 좌표를 만들지" 규칙만 등록.

			// 그 좌표 생성을 실제로 켠다 (별개 스위치!)
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glEnable(GL_TEXTURE_GEN_R);

			glMatrixMode(GL_TEXTURE);

			GLfloat m[16] = {
			    0.5f * (*mLightDirEyePtr)[0],
			    0.0f,
			    0.0f,
			    0.0f, // 열0
			    0.5f * (*mLightDirEyePtr)[1],
			    0.0f,
			    0.0f,
			    0.0f, // 열1
			    0.5f * (*mLightDirEyePtr)[2],
			    0.0f,
			    0.0f,
			    0.0f, // 열2
			    0.5f,
			    0.0f,
			    0.0f,
			    1.0f, // 열3
			};
			glEnable(GL_BLEND);
			// GL_DST_COLOR 가 Src이고 GL_ZERO가 Dst 아닌가?
			/*
			result 	= ramp × (GL_ZERO) + albedo × (GL_SRC_COLOR)
       				= ramp × 0         + albedo × ramp
       				= albedo × ramp    ✅  (순수 곱)
			*/
			glBlendFunc(GL_ZERO, GL_SRC_COLOR);   // = ramp × (이미 그린 albedo)
			glDepthFunc(GL_EQUAL);               // 같은 깊이 통과 (LESS면 전부 탈락 → 아무것도 안 그려짐!)
			glDepthMask(GL_FALSE);                // 깊이 재기록 방지
			glLoadMatrixf(m);
			glMatrixMode(GL_MODELVIEW);
		}

		void Unbind(const Material &mt) override
		{
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LESS);
			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);
			glDisable(GL_TEXTURE_1D);
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			glEnable(GL_LIGHTING);
		}
	};

	class BackfaceOutlineTechnique : public ITechnique
	{
	  private:
	  public:
		BackfaceOutlineTechnique()
		{
		}

		void Bind(const Material &mt) override
		{
			glPushMatrix();
			{
				auto& mScale = mt.outlineScale;
				glScalef(mScale, mScale, mScale);
			}
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D); glDisable(GL_TEXTURE_1D);
			glDisable(GL_TEXTURE_GEN_S); 
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);
			glColor3f(0.0f, 0.0f, 0.0f);
		}

		void Unbind(const Material &mt) override
		{
			glCullFace(GL_BACK);
			glDisable(GL_CULL_FACE);
			glEnable(GL_LIGHTING);
			glPopMatrix();
		}
	};



}; // namespace Metahuman

#endif //__METAHUMAN_MATERIAL_H__
