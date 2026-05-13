#ifndef __METAHUMAN_MODEL_IMP_H__
#define __METAHUMAN_MODEL_IMP_H__

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

// MSVC에서 M_PI 노출 — <cmath>보다 먼저 정의되어야 함
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>

#include "model.h"
#include "resource_management.h"
#include <glm/gtc/type_ptr.hpp>

namespace Metahuman
{

	// 텍스처가 매핑된 GLU 구체 모델
	// * Texture GL 핸들 소유권은 ResourceManagement에 있음 (여긴 borrowed pointer)
	// * quadric은 인스턴스마다 하나씩 보유 (소멸 시 gluDeleteQuadric)
	// * UV transform은 per-instance 상태 — Material 모듈 도입 전까지 여기서 보유
	class KeroroHead : public Model, public IUVTransformable
	{
	  private:
		GLUquadric *quadric = nullptr;
		Texture *texture = nullptr;
		UVTransform uv;
		double radius = 1.0;
		int slices = 32;
		int stacks = 16;

	  public:
		KeroroHead(Texture *texture,
		           double radius = 1.0,
		           int slices = 32,
		           int stacks = 16)
		    : texture(texture), radius(radius), slices(slices), stacks(stacks)
		{
			// quadric은 옵션 누적 컨테이너 — 도형 별로 매번 옵션 지정하지 않아도 되도록 분리됨
			quadric = gluNewQuadric();
			gluQuadricDrawStyle(quadric, GLU_FILL);      // 채워진 면
			gluQuadricNormals(quadric, GLU_SMOOTH);      // 부드러운 노멀 (조명 활성 시 사용)
			gluQuadricTexture(quadric, GL_TRUE);         // UV 좌표 자동 생성 — 텍스처 매핑 필수
			gluQuadricOrientation(quadric, GLU_OUTSIDE); // 노멀 바깥쪽 (구의 외부 셰이딩)
		}

		~KeroroHead() override
		{
			if (quadric)
			{
				gluDeleteQuadric(quadric);
				quadric = nullptr;
			}
		}

		// 추후에 텍스쳐 쓰는도형 안쓰는 도형 나눠서
		// 컴포넌트로 제작해서 확장 높이자.
		virtual void SetUV(const UVTransform &t) override
		{
			uv = t;
		}
		virtual const UVTransform &GetUV() const override
		{
			return uv;
		}

		void Draw() override
		{
			recalculateModelMatrix();

			glPushMatrix();
			glMultMatrixf(glm::value_ptr(modelMatrix));

			const GLuint id = texture ? texture->GetTextureID() : 0;
			if (id != 0)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, id);

				// UV 변환은 GL_TEXTURE 매트릭스 스택에 적용 — quadric이 만든 (s,t)에 곱해짐
				glMatrixMode(GL_TEXTURE);
				glPushMatrix();
				glLoadIdentity();
				glTranslatef(uv.offset.x, uv.offset.y, 0.0f);
				glRotatef(uv.rotationDeg, 0.0f, 0.0f, 1.0f);
				glScalef(uv.scale.x, uv.scale.y, 1.0f);
				glMatrixMode(GL_MODELVIEW); // 즉시 복원 — 다른 모델에 영향 안 가도록
			}

			glColor3f(1.0f, 1.0f, 1.0f);
			gluSphere(quadric, radius, slices, stacks);

			if (id != 0)
			{
				glMatrixMode(GL_TEXTURE);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glDisable(GL_TEXTURE_2D);
			}

			glPopMatrix();
		}
	};

	class KeroroBody : public ParametricGeometry, public IUVTransformable
	{
	  private:
		Texture *texture = nullptr;
		UVTransform uv;

	  public:
		KeroroBody(Texture *texture = nullptr,
		           size_t phiRes = 32,                    // u 분할 (경도) — 회전 부드러움
		           size_t thetaRes = 16)                  // v 분할 (위도) — 프로파일 곡선
		    : ParametricGeometry(0.0, 2.0 * M_PI, phiRes, // u = φ ∈ [0, 2π]
		                         0.1, M_PI, thetaRes),    // v = θ ∈ [0.1, π], 꼭대기 cusp 회피
		      texture(texture)
		{
			build(); // 가상 SurfaceFunction이 준비된 후에 호출
		}

		virtual void SetUV(const UVTransform &t) override
		{
			uv = t;
		}
		virtual const UVTransform &GetUV() const override
		{
			return uv;
		}

		virtual glm::vec4 SurfaceFunction(double u, double v) const override
		{
			const float phi = (float)(u);
			const float theta = (float)(v);
			const float hR = 0.5f * (1.0f - cosf(theta)) * sinf(theta);
			return glm::vec4(hR * cosf(phi),
			                 cosf(theta), // y = cos θ (Y축 높이)
			                 hR * sinf(phi),
			                 1.0f);
		}

		void Draw() override
		{
			const GLuint id = texture ? texture->GetTextureID() : 0;
			if (id != 0)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, id);

				// UV 변환은 GL_TEXTURE 매트릭스 스택에 적용 — 격자가 만든 (s,t)에 곱해짐
				glMatrixMode(GL_TEXTURE);
				glPushMatrix();
				glLoadIdentity();
				glTranslatef(uv.offset.x, uv.offset.y, 0.0f);
				glRotatef(uv.rotationDeg, 0.0f, 0.0f, 1.0f);
				glScalef(uv.scale.x, uv.scale.y, 1.0f);
				glMatrixMode(GL_MODELVIEW); // 즉시 복원 — 다른 모델 영향 방지
			}

			glColor3f(1.0f, 1.0f, 1.0f);
			ParametricGeometry::Draw(); // TRS 적용 + 격자 메쉬 렌더

			if (id != 0)
			{
				glMatrixMode(GL_TEXTURE);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glDisable(GL_TEXTURE_2D);
			}
		}
	};

} // namespace Metahuman

#endif //__METAHUMAN_MODEL_IMP_H__
