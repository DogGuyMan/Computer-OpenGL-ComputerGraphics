#ifndef __METAHUMAN_MODEL_H__
#define __METAHUMAN_MODEL_H__
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include "transformable.h"
#include <glm/glm.hpp>

namespace Metahuman {
	class Model : public ITransformable {
	protected :
		// TRS 분리 저장
		glm::mat4 modelTMatrix = glm::mat4(1.0f);
		glm::mat4 modelRMatrix = glm::mat4(1.0f);
		glm::mat4 modelSMatrix = glm::mat4(1.0f);

		// 최종 합성 행렬 (지연 평가)
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		bool dirtyFlag = true;

		// dirty일 때만 modelMatrix 재계산
		void recalculateModelMatrix();

	public :
		Model();
		virtual ~Model();
		virtual void Draw();

		const glm::mat4& GetModelMatrix();

		/* Transformable — 모두 절대값 설정 (누적 아님) */
		void Translate(const glm::fvec3 &pos) override;
		void Rotate(const glm::fvec3 &eulerDeg) override;
		void Scale(const glm::fvec3 &factor) override;
		void SetTransform(const Metahuman::PODTransform& t);
	};
};

#endif//__METAHUMAN_MODEL_H__
