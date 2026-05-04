#ifndef __METAHUMAN_INTERFACES_H__
#define __METAHUMAN_INTERFACES_H__

#include <glm/glm.hpp>

namespace Metahuman
{
	struct PODTransform {
		glm::vec3 translate {0.0f, 0.0f, 0.0f};
		glm::vec3 eulerDeg {0.0f, 0.0f, 0.0f}; // XYZ, DEG
		glm::vec3 scale {1.0f, 1.0f,1.0f};
	};

	// 텍스처 좌표 변환 (per-instance 상태)
	// quadric/mesh가 자동 생성한 (s,t) UV에 GL_TEXTURE 매트릭스로 곱해진다.
	// Material 모듈 도입 전까지는 텍스처를 쓰는 모델 자식 클래스가 직접 보유.
	struct UVTransform {
		glm::vec2 offset {0.0f, 0.0f};
		glm::vec2 scale  {1.0f, 1.0f};
		float rotationDeg = 0.0f;
	};

	class ITranslatable
	{
	protected:
		ITranslatable() = default;
	public:
		virtual ~ITranslatable() = default;
		virtual void Translate(const glm::fvec3 &pos) = 0;
		ITranslatable(const ITranslatable &) = delete;
		ITranslatable operator=(const ITranslatable &) = delete;
		ITranslatable(ITranslatable &&) = delete;
		ITranslatable operator=(ITranslatable &) = delete;
	};

	class IRotatable
	{
	protected:
		IRotatable() = default;
	public:
		virtual ~IRotatable() = default;
		virtual void Rotate(const glm::fvec3 &eulerDeg) = 0;
		IRotatable(const IRotatable &) = delete;
		IRotatable operator=(const IRotatable &) = delete;
		IRotatable(IRotatable &&) = delete;
		IRotatable operator=(IRotatable &) = delete;
	};

	class IScaleable
	{
	protected:
		IScaleable() = default;
	public:
		virtual ~IScaleable() = default;
		virtual void Scale(const glm::fvec3 &factor) = 0;
		IScaleable(const IScaleable &) = delete;
		IScaleable operator=(const IScaleable &) = delete;
		IScaleable(IScaleable &&) = delete;
		IScaleable operator=(IScaleable &) = delete;
	};

	class ITransformable : public ITranslatable, IRotatable, IScaleable
	{
	protected:
		ITransformable() = default;
	public:
		virtual ~ITransformable() = default;
		ITransformable(const ITransformable &) = delete;
		ITransformable operator=(const ITransformable &) = delete;
		ITransformable(ITransformable &&) = delete;
		ITransformable operator=(ITransformable &) = delete;
	};
}
#endif //__METAHUMAN_INTERFACES_H__