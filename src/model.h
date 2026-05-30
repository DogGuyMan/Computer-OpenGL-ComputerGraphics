#ifndef __METAHUMAN_MODEL_H__
#define __METAHUMAN_MODEL_H__
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include "transformable.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace Metahuman
{
	
	class Model : public ITransformable
	{
	  protected:
		// POD 형태의 변환 상태 — GetPODTransform()의 const& 반환을 위해 보관.
		// Translate/Rotate/Scale이 각 필드를 갱신해 아래 TRS 행렬과 항상 정합 유지.
		TransformValue trans;

		// TRS 분리 저장
		glm::mat4 modelTMatrix = glm::mat4(1.0f);
		glm::mat4 modelRMatrix = glm::mat4(1.0f);
		glm::mat4 modelSMatrix = glm::mat4(1.0f);

		// 최종 합성 행렬 (지연 평가)
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		bool dirtyFlag = true;

		// dirty일 때만 modelMatrix 재계산
		void recalculateModelMatrix();

	  public:
		Model();
		virtual ~Model();
		virtual void Draw();

		const glm::mat4 &GetModelMatrix();
		virtual const TransformValue &GetPODTransform() const override;
		virtual void SetTransform(const TransformValue &p) override;

		/* Transformable — 모두 절대값 설정 (누적 아님) */
		void Translate(const glm::fvec3 &pos) override;
		void Rotate(const glm::fvec3 &eulerDeg) override;
		void Scale(const glm::fvec3 &factor) override;
	};

	class ParametricGeometry : public Model, public IParametricTransformable
	{
	  protected:
		// u/v 범위 + 격자 해상도. SetParametricParams로 교체 시 build() 재호출.
		ParametricValue params;

		// (uRes+1) × (vRes+1) 격자 캐시. build()에서 한 번 채우고 매 프레임 재사용
		std::vector<glm::vec4> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;

		// 격자를 SurfaceFunction으로 샘플링하여 정점/노멀/UV 캐시 생성.
		// 노멀은 finite difference로 계산 (퇴화 시 (0,1,0) fallback).
		void build();

		// 캐시된 정점 하나(법선+UV+위치)를 GL에 emit. Draw()의 셀 꼭짓점 출력에 재사용.
		void emitVertex(size_t idx) const;

		// 도형별 매개변수 곡면 함수. (u, v) -> (x, y, z, 1)
		virtual glm::vec4 SurfaceFunction(double u, double v) const = 0;

	  public:
		ParametricGeometry(double u_start, double u_end, size_t u_res,
		                   double v_start, double v_end, size_t v_res);
		~ParametricGeometry() override = default;

		// TRS 적용 후 GL_QUAD_STRIP으로 격자 렌더
		void Draw() override;

		// GetPODTransform/SetTransform은 Model 구현을 그대로 상속 (TRS는 메쉬와 무관).

		// IParametricTransformable — u/v 범위*해상도를 POD 구조체로 노출.
		// SetParametricParams는 값 교체 후 build()로 메쉬를 재생성한다.
		void SetParametricParams(const ParametricValue &p) override;
		const ParametricValue &GetParametricParams() const override;
	};

} // namespace Metahuman

#endif //__METAHUMAN_MODEL_H__
