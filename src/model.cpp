#include "model.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Metahuman
{
	Model::Model()
	{
	}

	Model::~Model()
	{
	}

	void Model::recalculateModelMatrix()
	{
		if (!dirtyFlag)
			return;
		// TRS 순서: Translation * Rotation * Scale
		modelMatrix = modelTMatrix * modelRMatrix * modelSMatrix;
		dirtyFlag = false;
	}

	const glm::mat4 &Model::GetModelMatrix()
	{
		recalculateModelMatrix();
		return modelMatrix;
	}

	void Model::Draw()
	{
		recalculateModelMatrix();

		glPushMatrix();
		glMultMatrixf(glm::value_ptr(modelMatrix));

		// 기본 도형 그리기
		glColor3f(1.0f, 0.0f, 0.0f);
		glutWireSphere(1, 20, 20);

		glPopMatrix();
	}

	void Model::Translate(const glm::fvec3 &pos)
	{
		modelTMatrix = glm::translate(glm::mat4(1.0f), pos);
		dirtyFlag = true;
	}

	void Model::Rotate(const glm::fvec3 &eulerDeg)
	{
		const glm::vec3 r = glm::radians(eulerDeg);
		const glm::mat4 rx = glm::rotate(glm::mat4(1.0f), r.x, glm::vec3(1.0f, 0.0f, 0.0f));
		const glm::mat4 ry = glm::rotate(glm::mat4(1.0f), r.y, glm::vec3(0.0f, 1.0f, 0.0f));
		const glm::mat4 rz = glm::rotate(glm::mat4(1.0f), r.z, glm::vec3(0.0f, 0.0f, 1.0f));
		modelRMatrix = rx * ry * rz;
		dirtyFlag = true;
	}

	void Model::Scale(const glm::fvec3 &factor)
	{
		modelSMatrix = glm::scale(glm::mat4(1.0f), factor);
		dirtyFlag = true;
	}

	void Model::SetTransform(const Metahuman::PODTransform &t)
	{
		Translate(t.translate);
		Rotate(t.eulerDeg);
		Scale(t.scale);
	}

	ParametricGeometry::ParametricGeometry(double u_start, double u_end, size_t u_res,
	                                       double v_start, double v_end, size_t v_res)
	    : uStart(u_start), uEnd(u_end),
	      vStart(v_start), vEnd(v_end),
	      uRes(u_res), vRes(v_res)
	{
		// build()는 자식 생성자에서 호출 — SurfaceFunction이 순수가상이라
		// 베이스 생성자 시점에선 자식 버전이 디스패치되지 않음
	}

	void ParametricGeometry::build()
	{
		const size_t uCount = uRes + 1;
		const size_t vCount = vRes + 1;
		vertices.resize(uCount * vCount);
		normals.resize(uCount * vCount);
		texCoords.resize(uCount * vCount);

		const double du = (uEnd - uStart) / static_cast<double>(uRes);
		const double dv = (vEnd - vStart) / static_cast<double>(vRes);
		// finite difference 스텝 — 격자 간격의 1%면 정밀도/안정성 절충
		const double eps = std::min(du, dv) * 0.01;

		for (size_t i = 0; i < uCount; ++i)
		{
			const double u = uStart + du * static_cast<double>(i);
			for (size_t j = 0; j < vCount; ++j)
			{
				const double v = vStart + dv * static_cast<double>(j);
				const size_t idx = i * vCount + j;

				vertices[idx] = SurfaceFunction(u, v);
				texCoords[idx] = glm::vec2(
				    static_cast<float>(i) / static_cast<float>(uRes),
				    static_cast<float>(j) / static_cast<float>(vRes));

				const glm::vec4 pu = SurfaceFunction(u + eps, v);
				const glm::vec4 pv = SurfaceFunction(u, v + eps);
				const glm::vec3 dPdu = glm::vec3(pu - vertices[idx]);
				const glm::vec3 dPdv = glm::vec3(pv - vertices[idx]);
				const glm::vec3 cross = glm::cross(dPdu, dPdv);
				const float len = glm::length(cross);
				// 퇴화(특이점) 시 안전한 fallback. v_start로 cusp를 피하라는 것이 1차 처방
				normals[idx] = (len > 1e-6f) ? (cross / len) : glm::vec3(0.0f, 1.0f, 0.0f);
			}
		}
	}

	void ParametricGeometry::Draw()
	{
		recalculateModelMatrix();

		glPushMatrix();
		glMultMatrixf(glm::value_ptr(modelMatrix));

		const size_t vCount = vRes + 1;
		for (size_t i = 0; i < uRes; ++i)
		{
			glBegin(GL_QUAD_STRIP);
			for (size_t j = 0; j <= vRes; ++j)
			{
				const size_t idx0 = i * vCount + j;
				const size_t idx1 = (i + 1) * vCount + j;

				glNormal3fv(glm::value_ptr(normals[idx0]));
				glTexCoord2fv(glm::value_ptr(texCoords[idx0]));
				glVertex4fv(glm::value_ptr(vertices[idx0]));

				glNormal3fv(glm::value_ptr(normals[idx1]));
				glTexCoord2fv(glm::value_ptr(texCoords[idx1]));
				glVertex4fv(glm::value_ptr(vertices[idx1]));
			}
			glEnd();
		}

		glPopMatrix();
	}

} // namespace Metahuman