#ifndef __METAHUMAN_MODEL_IMP_H__
#define __METAHUMAN_MODEL_IMP_H__

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include "model.h"
#include "resource_management.h"
#include <glm/gtc/type_ptr.hpp>

namespace Metahuman {

	// 텍스처가 매핑된 GLU 구체 모델
	// * Texture GL 핸들 소유권은 ResourceManagement에 있음 (여긴 borrowed pointer)
	// * quadric은 인스턴스마다 하나씩 보유 (소멸 시 gluDeleteQuadric)
	// * UV transform은 per-instance 상태 — Material 모듈 도입 전까지 여기서 보유
	class TexturedSphere : public Model {
	private:
		GLUquadric* quadric = nullptr;
		Texture* texture = nullptr;
		UVTransform uv;
		double radius = 1.0;
		int slices = 32;
		int stacks = 16;

	public:
		TexturedSphere(Texture* texture,
		                        double radius = 1.0,
		                        int slices = 32,
		                        int stacks = 16)
			: texture(texture), radius(radius), slices(slices), stacks(stacks)
		{
			// quadric은 옵션 누적 컨테이너 — 도형 별로 매번 옵션 지정하지 않아도 되도록 분리됨
			quadric = gluNewQuadric();
			gluQuadricDrawStyle(quadric, GLU_FILL);       // 채워진 면
			gluQuadricNormals(quadric, GLU_SMOOTH);       // 부드러운 노멀 (조명 활성 시 사용)
			gluQuadricTexture(quadric, GL_TRUE);          // UV 좌표 자동 생성 — 텍스처 매핑 필수
			gluQuadricOrientation(quadric, GLU_OUTSIDE);  // 노멀 바깥쪽 (구의 외부 셰이딩)
		}

		~TexturedSphere() override {
			if (quadric) {
				gluDeleteQuadric(quadric);
				quadric = nullptr;
			}
		}
		
		// 추후에 텍스쳐 쓰는도형 안쓰는 도형 나눠서
		// 컴포넌트로 제작해서 확장 높이자.
		void SetUV(const UVTransform& t) { uv = t; }
		const UVTransform& GetUV() const { return uv; }

		void Draw() override {
			recalculateModelMatrix();

			glPushMatrix();
			glMultMatrixf(glm::value_ptr(modelMatrix));

			const GLuint id = texture ? texture->GetTextureID() : 0;
			if (id != 0) {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, id);

				// UV 변환은 GL_TEXTURE 매트릭스 스택에 적용 — quadric이 만든 (s,t)에 곱해짐
				glMatrixMode(GL_TEXTURE);
				glPushMatrix();
				glLoadIdentity();
				glTranslatef(uv.offset.x, uv.offset.y, 0.0f);
				glRotatef(uv.rotationDeg, 0.0f, 0.0f, 1.0f);
				glScalef(uv.scale.x, uv.scale.y, 1.0f);
				glMatrixMode(GL_MODELVIEW);  // 즉시 복원 — 다른 모델에 영향 안 가도록
			}

			glColor3f(1.0f, 1.0f, 1.0f);
			gluSphere(quadric, radius, slices, stacks);

			if (id != 0) {
				glMatrixMode(GL_TEXTURE);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glDisable(GL_TEXTURE_2D);
			}

			glPopMatrix();
		}
	};

} // namespace Metahuman

#endif //__METAHUMAN_MODEL_IMP_H__
