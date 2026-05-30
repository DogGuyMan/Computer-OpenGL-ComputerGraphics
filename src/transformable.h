#ifndef __METAHUMAN_INTERFACES_H__
#define __METAHUMAN_INTERFACES_H__

#include <glm/glm.hpp>
#include <cstddef> // size_t

namespace Metahuman
{
	struct TransformValue
	{
		glm::vec3 translate{0.0f, 0.0f, 0.0f};
		glm::vec3 eulerDeg{0.0f, 0.0f, 0.0f}; // XYZ, DEG
		glm::vec3 scale{1.0f, 1.0f, 1.0f};
	};

	// 텍스처 좌표 변환 (per-instance 상태)
	// quadric/mesh가 자동 생성한 (s,t) UV에 GL_TEXTURE 매트릭스로 곱해진다.
	// Material 모듈 도입 전까지는 텍스처를 쓰는 모델 자식 클래스가 직접 보유.
	struct UVValue
	{
		glm::vec2 offset{0.0f, 0.0f};
		glm::vec2 scale{1.0f, 1.0f};
		float rotationDeg = 0.0f;
	};

	// 매개변수 곡면의 u/v 범위 + 격자 해상도 (per-instance 상태)
	// ParametricGeometry가 보유. 값이 바뀌면 build()로 메쉬를 재생성한다.
	struct ParametricValue
	{
		double uStart = 0.0, uEnd = 1.0;
		double vStart = 0.0, vEnd = 1.0;
		size_t uRes = 16, vRes = 16;
	};

	// 1-sheet hyperboloid 형상 파라미터 (Paul Bourke, paulbourke.net/geometry/hyperboloid)
	//   단면 반경 r(s) = radius * \sqrt(shape^2 + s^2) / \sqrt(shape^2 + 1)
	//   shape(=d) -> 0: 이중원뿔,  shape : 원기둥에 수렴
	struct HyperboloidValue
	{
		float radius = 1.0f; // 양 끝(s=±1)에서의 단면 반경
		float height = 1.0f; // 축(높이) 방향 스케일
		float shape = 1.0f;  // 허리 조임 정도 d (0=원뿔, 클수록 원기둥)
	};

	class IUVTransformable
	{
	  protected:
		IUVTransformable() = default;

	  public:
		virtual ~IUVTransformable() = default;
		virtual void SetUV(const UVValue &t) = 0;
		virtual const UVValue &GetUV() const = 0;
		IUVTransformable(const IUVTransformable &) = delete;
		IUVTransformable operator=(const IUVTransformable &) = delete;
		IUVTransformable(IUVTransformable &&) = delete;
		IUVTransformable operator=(IUVTransformable &) = delete;
	};

	// 매개변수 곡면의 u/v 범위*해상도를 POD 구조체로 노출하는 인터페이스.
	// UI(ImGui) 측은 이 인터페이스만 알면 되고, friend 없이 접근 가능하다.
	// SetParametricParams 구현체는 값 교체 후 메쉬를 재생성(build)할 책임이 있다.
	class IParametricTransformable
	{
	  protected:
		IParametricTransformable() = default;

	  public:
		virtual ~IParametricTransformable() = default;
		virtual void SetParametricParams(const ParametricValue &p) = 0;
		virtual const ParametricValue &GetParametricParams() const = 0;
		IParametricTransformable(const IParametricTransformable &) = delete;
		IParametricTransformable operator=(const IParametricTransformable &) = delete;
		IParametricTransformable(IParametricTransformable &&) = delete;
		IParametricTransformable operator=(IParametricTransformable &) = delete;
	};

	// Hyperboloid 형상 파라미터(radius/height/shape)를 POD로 노출하는 인터페이스.
	// SetHyperboloidParams 구현체는 값 교체 후 메쉬를 재생성(build)할 책임이 있다.
	class IHyperboloidTransformable
	{
	  protected:
		IHyperboloidTransformable() = default;

	  public:
		virtual ~IHyperboloidTransformable() = default;
		virtual void SetHyperboloidParams(const HyperboloidValue &p) = 0;
		virtual const HyperboloidValue &GetHyperboloidParams() const = 0;
		IHyperboloidTransformable(const IHyperboloidTransformable &) = delete;
		IHyperboloidTransformable operator=(const IHyperboloidTransformable &) = delete;
		IHyperboloidTransformable(IHyperboloidTransformable &&) = delete;
		IHyperboloidTransformable operator=(IHyperboloidTransformable &) = delete;
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
		virtual const TransformValue &GetPODTransform() const = 0;
		virtual void SetTransform(const TransformValue &p) = 0;
		ITransformable(const ITransformable &) = delete;
		ITransformable operator=(const ITransformable &) = delete;
		ITransformable(ITransformable &&) = delete;
		ITransformable operator=(ITransformable &) = delete;
	};
} // namespace Metahuman
#endif //__METAHUMAN_INTERFACES_H__