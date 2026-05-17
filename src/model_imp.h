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
		UVValue uv;
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
			trans = DefaultTransform();
			uv = DefaultUV(); // 초기 UV 기본값 — 모델이 스스로 기본값으로 출발
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
		virtual void SetUV(const UVValue &t) override
		{
			uv = t;
		}
		virtual const UVValue &GetUV() const override
		{
			return uv;
		}

		// 프로그램 시작 시 ImGui(g_xforms/g_uvs)와 모델 상태를 동기화하기 위한 초기 기본값
		static TransformValue DefaultTransform()
		{
			TransformValue t;
			t.translate = glm::vec3(0.0f, 0.0f, 0.0f);
			t.eulerDeg = glm::vec3(90.0f, 0.0f, 0.0f);
			t.scale = glm::vec3(1.0f, 1.0f, 0.95f);
			return t;
		}
		static UVValue DefaultUV()
		{
			UVValue u;
			u.offset = glm::vec2(0.0f, -0.550f);
			u.scale = glm::vec2(3.8f, 1.95f);
			return u;
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
		UVValue uv;

	  public:
		KeroroBody(Texture *texture = nullptr,
		           size_t phiRes = 32,                    // u 분할 (경도) — 회전 부드러움
		           size_t thetaRes = 16)                  // v 분할 (위도) — 프로파일 곡선
		    : ParametricGeometry(0.0, 2.0 * M_PI, phiRes, // u = φ ∈ [0, 2π]
		                         0.1, M_PI, thetaRes),    // v = θ ∈ [0.1, π], 꼭대기 cusp 회피
		      texture(texture)
		{
			trans = DefaultTransform();
			uv = DefaultUV(); // 초기 UV 기본값 — 모델이 스스로 기본값으로 출발
			build();          // 가상 SurfaceFunction이 준비된 후에 호출
		}

		virtual void SetUV(const UVValue &t) override
		{
			uv = t;
		}
		virtual const UVValue &GetUV() const override
		{
			return uv;
		}

		// 프로그램 시작 시 ImGui(g_xforms/g_uvs)와 모델 상태를 동기화하기 위한 초기 기본값
		static TransformValue DefaultTransform()
		{
			TransformValue t;
			t.translate = glm::vec3(0.0f, -1.2f, 0.0f);
			t.eulerDeg = glm::vec3(0.0f, 0.0f, 0.0f);
			t.scale = glm::vec3(0.7f, 0.65f, 0.7f);
			return t;
		}
		static UVValue DefaultUV()
		{
			UVValue u;
			u.offset = glm::vec2(-0.475f, -0.750f);
			u.scale = glm::vec2(2.65f, 2.20f);
			return u;
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

	class KeroroHat : public ParametricGeometry, public IUVTransformable, public IHyperboloidTransformable
	{
	  private:
		Texture *texture = nullptr;
		UVValue uv;
		// 형상 파라미터 — radius / height / shape(d)
		HyperboloidValue hyper{2.074f, 2.7f, 0.52f};

	  public:
		KeroroHat(Texture *texture = nullptr,
		          size_t uRes = 32,                       // u 분할 (수평, 경도) — 회전 부드러움
		          size_t vRes = 16)                       // v 분할 (수직) — 프로파일 곡선
		    : ParametricGeometry(0.0, 2.0 * M_PI, uRes,   // u = φ ∈ [0, 2π]
		                         0.06, 0.5, vRes),        // v ∈ [0.06, 0.5], xz 평면 아래쪽 절반
		      texture(texture)
		{
			trans = DefaultTransform();
			uv = DefaultUV(); // 초기 UV 기본값 — 모델이 스스로 기본값으로 출발
			build();
		}

		virtual void SetUV(const UVValue &t) override
		{
			uv = t;
		}
		virtual const UVValue &GetUV() const override
		{
			return uv;
		}

		// 프로그램 시작 시 ImGui(g_xforms/g_uvs)와 모델 상태를 동기화하기 위한 초기 기본값
		static TransformValue DefaultTransform()
		{
			TransformValue t;
			t.translate = glm::vec3(0.0f, 0.5f, 0.0f);
			t.eulerDeg = glm::vec3(0.0f, -173.0f, 0.0f);
			t.scale = glm::vec3(1.0f, 1.0f, 1.0f);
			return t;
		}
		static UVValue DefaultUV()
		{
			UVValue u;
			u.offset = glm::vec2(0.0f, -0.050f);
			u.scale = glm::vec2(1.2f, 1.0f);
			return u;
		}

		// IHyperboloidTransformable — 형상 파라미터를 POD로 노출.
		// Set은 값 교체 후 build()로 메쉬를 재생성한다.
		virtual void SetHyperboloidParams(const HyperboloidValue &p) override
		{
			hyper = p;
			build();
		}
		virtual const HyperboloidValue &GetHyperboloidParams() const override
		{
			return hyper;
		}

		virtual glm::vec4 SurfaceFunction(double u, double v) const override
		{
			// Paul Bourke 1-sheet hyperboloid (paulbourke.net/geometry/hyperboloid)
			//   단면 반경 r(s) = radius · √(d² + s²) / √(d² + 1),   s = v
			// u → φ (수평 경도),  v → s (수직 매개변수)
			// 이 (u, v) 배치라야 ∂P/∂u × ∂P/∂v 가 outward normal이 됨
			const float phi = (float)(u);
			const float s = (float)(v);
			const float d = hyper.shape;
			const float r = hyper.radius * sqrtf(d * d + s * s) / sqrtf(d * d + 1.0f);
			return glm::vec4(r * cosf(phi),
			                 -hyper.height * s,     // y = -h·s (xz 평면 아래쪽으로 벌어짐)
			                 r * sinf(phi),
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
				glMatrixMode(GL_MODELVIEW);
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
