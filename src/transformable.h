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