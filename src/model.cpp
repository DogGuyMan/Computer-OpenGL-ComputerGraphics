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
		trans.translate = pos;
		modelTMatrix = glm::translate(glm::mat4(1.0f), pos);
		dirtyFlag = true;
	}

	void Model::Rotate(const glm::fvec3 &eulerDeg)
	{
		trans.eulerDeg = eulerDeg;
		const glm::vec3 r = glm::radians(eulerDeg);
		const glm::mat4 rx = glm::rotate(glm::mat4(1.0f), r.x, glm::vec3(1.0f, 0.0f, 0.0f));
		const glm::mat4 ry = glm::rotate(glm::mat4(1.0f), r.y, glm::vec3(0.0f, 1.0f, 0.0f));
		const glm::mat4 rz = glm::rotate(glm::mat4(1.0f), r.z, glm::vec3(0.0f, 0.0f, 1.0f));
		modelRMatrix = rx * ry * rz;
		dirtyFlag = true;
	}

	void Model::Scale(const glm::fvec3 &factor)
	{
		trans.scale = factor;
		modelSMatrix = glm::scale(glm::mat4(1.0f), factor);
		dirtyFlag = true;
	}

	void Model::SetTransform(const Metahuman::TransformValue &t)
	{
		// Translate/Rotate/Scale이 각자 podtransform 필드를 갱신하므로 자동 정합.
		Translate(t.translate);
		Rotate(t.eulerDeg);
		Scale(t.scale);
	}

	const TransformValue &Model::GetPODTransform() const
	{
		return trans;
	}

	ParametricGeometry::ParametricGeometry(double u_start, double u_end, size_t u_res,
	                                       double v_start, double v_end, size_t v_res)
	{
		params.uStart = u_start;
		params.uEnd = u_end;
		params.uRes = u_res;
		params.vStart = v_start;
		params.vEnd = v_end;
		params.vRes = v_res;
		// build()는 자식 생성자에서 호출 — SurfaceFunction이 순수가상이라
		// 베이스 생성자 시점에선 자식 버전이 디스패치되지 않음
	}

	void ParametricGeometry::build()
	{
		const size_t uCount = params.uRes + 1;
		const size_t vCount = params.vRes + 1;
		vertices.resize(uCount * vCount);
		normals.resize(uCount * vCount);
		texCoords.resize(uCount * vCount);

		const double du = (params.uEnd - params.uStart) / static_cast<double>(params.uRes);
		const double dv = (params.vEnd - params.vStart) / static_cast<double>(params.vRes);
		const double epsU = du * 0.01;
		const double epsV = dv * 0.01;

		for (size_t i = 0; i < uCount; ++i)
		{
			const double u = params.uStart + du * static_cast<double>(i);
			for (size_t j = 0; j < vCount; ++j)
			{
				const double v = params.vStart + dv * static_cast<double>(j);
				const size_t idx = i * vCount + j;

				vertices[idx] = SurfaceFunction(u, v);
				texCoords[idx] = glm::vec2(
				    static_cast<float>(i) / static_cast<float>(params.uRes),
				    static_cast<float>(j) / static_cast<float>(params.vRes));

				const glm::vec4 pu = SurfaceFunction(u + epsU, v);
				const glm::vec4 pv = SurfaceFunction(u, v + epsV);
				const glm::vec3 dPdu = glm::vec3(pu - vertices[idx]);
				const glm::vec3 dPdv = glm::vec3(pv - vertices[idx]);
				const glm::vec3 cross = glm::cross(dPdu, dPdv);
				const float len = glm::length(cross);
				// 퇴화(특이점) 시 안전한 fallback. v_start로 cusp를 피하라는 것이 1차 처방
				normals[idx] = (len > 1e-6f) ? (cross / len) : glm::vec3(0.0f, 1.0f, 0.0f);
			}
		}
	}

	void ParametricGeometry::emitVertex(size_t idx) const
	{
		glNormal3fv(glm::value_ptr(normals[idx]));
		glTexCoord2fv(glm::value_ptr(texCoords[idx]));
		glVertex4fv(glm::value_ptr(vertices[idx]));
	}

	void ParametricGeometry::Draw()
	{
		recalculateModelMatrix();

		glPushMatrix();
		glMultMatrixf(glm::value_ptr(modelMatrix));

		const size_t vCount = params.vRes + 1;

		for (size_t i = 0; i < params.uRes; ++i)
		{
			for (size_t j = 0; j < params.vRes; ++j)
			{
				const size_t i0 = i * vCount;
				const size_t i1 = (i + 1) * vCount;

				glBegin(GL_POLYGON);
				emitVertex(i0 + j);     // (i,   j)
				emitVertex(i1 + j);     // (i+1, j)
				emitVertex(i1 + j + 1); // (i+1, j+1)
				emitVertex(i0 + j + 1); // (i,   j+1)
				glEnd();
			}
		}

		glPopMatrix();
	}

	void ParametricGeometry::SetParametricParams(const ParametricValue &p)
	{
		params = p;
		build();
	}

	const ParametricValue &ParametricGeometry::GetParametricParams() const
	{
		return params;
	}

} // namespace Metahuman