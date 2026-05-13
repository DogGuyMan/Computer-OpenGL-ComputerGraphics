# WaterDrop Parametric Surface 구현 정리집

> 강의 과제(`chapter6_extra_1` → `chapter7`)에서 물방울 형태의 회전체를 parametric surface로 만든 과정의 전 기록을 시간순으로 정리한 문서. 출처는 두 개의 jsonl 세션 로그(`b8a9ada8-3169-492b-b69b-ef1f58189b39`, `c321bf36-2cd4-4e59-a63e-823bcd303353`)와 PDF 자료 *Code journal: water droplet model* (Mariana Ávalos Arce, 2019).
>
> 7장(Piriform Surface)에서는 Paul Bourke의 또 다른 배 모양 회전체와 그것을 Y축 회전체로 변형할 때 발생하는 아티팩트를 함께 다룬다.

---

## 1. 출처와 수학적 배경

### 1.1 두 개의 1차 자료

| 자료 | 핵심 아이디어 |
|---|---|
| **Mariana Ávalos Arce, *Code journal: water droplet model* (2019)** | 단위원에서 출발 → top point `(0, R)`까지의 거리로 반지름을 변조하여 물방울 단면(2D)을 만든다. Unity 3D 워터 셰이더용 |
| **Paul Bourke, *Teardrop* (paulbourke.net/geometry/teardrop)** | 닫힌 형식(closed-form) 회전체 공식. 수평 반경 = `(1 − cos θ) · sin θ` |

두 자료는 같은 도형을 다른 방식으로 도출한다. 최종 코드는 Bourke 공식을 채택하되 Ávalos의 PDF가 도입한 `fatness` 파라미터화 아이디어를 보존한 형태다.

### 1.2 PDF 공식의 도출 흐름 (Ávalos)

PDF의 논리는 다음 4단계로 진행된다.

**Step 1 — 기준 원** (Figure 1)

```
ParametricPlot[{Cos[t], Sin[t]}, {t, 0, 2π}]
```

단위원에 top point `(0, radius)`을 표시한다.

**Step 2 — 거리를 새 반경으로** (Figure 2, fatness=1.0)

원 위의 각 점 `(R·cos t, R·sin t)`에서 top point `(x₀, y₀) = (0, R)`까지의 거리를 계산하여 그 거리를 **새 수평 반경**으로 사용한다.

```
new_r(t) = sqrt((x₀ − R·cos t)² + (y₀ − R·sin t)²)
x(t) = new_r(t) · R · cos(t)
y(t) = R · sin(t)
```

이 단계만으로도 위쪽이 좁고 아래쪽이 넓은 형태가 나오지만, 너무 부풀어 있다.

**Step 3 — fatness 도입** (Figure 3, fatness=0.5)

`new_r`이 너무 강하므로 0~1 사이의 스케일 팩터(`fatness`)를 곱해 변조 강도를 줄인다.

```
x(t) = fatness · new_r(t) · R · cos(t)
```

**Step 4 — 극좌표 단순화** (Figure 4, equation 1)

PDF는 마지막에 더 간결한 극형식도 제시한다.

```
r = fatness · sin((circleRadius / 2) · t)
x = circleRadius · r · sin(t)
y = circleRadius · cos(t)
```

이게 PDF의 최종 결론(equation 1)이고, 코드 구현 1차 시도에 실제로 들어간 식이다.

### 1.3 Bourke 공식 (회전체 closed-form)

Paul Bourke의 teardrop은 회전체를 직접 정의한다.

```
x = 0.5 · (1 − cos θ) · sin θ · cos φ
y = 0.5 · (1 − cos θ) · sin θ · sin φ
z = cos θ
```

축 변환에 따라 (위 식은 z를 높이로 쓰지만, 그래픽스에서는 보통 y가 높이) 다음과 같이 쓸 수 있다.

```
horizontalR(θ) = (1 − cos θ) · sin θ
x = horizontalR · cos φ
y = cos θ                    ← 높이
z = horizontalR · sin φ
```

**핵심은 `(1 − cos θ) · sin θ` 라는 단일 함수**. 이 함수는

- `θ = 0`에서 `(1−1)·0 = 0` → 한 점으로 수렴(뾰족한 꼭대기)
- `θ ≈ 2.2 rad`에서 최대값
- `θ = π`에서 `(1−(−1))·0 = 0` → 다시 한 점으로 수렴(둥근 바닥의 한 점)

이라는 자연스러운 물방울 프로파일을 한 줄로 표현한다.

---

## 2. 구현 진화 — 4단계의 시행착오

### Stage A — 첫 시도: Ávalos PDF 거리 공식 직접 3D 확장

```cpp
// 첫 구현: PDF의 거리식을 3D로 확장
virtual vmath::vec4 SurfaceFunction(double u, double v) const override
{
    float R = mRadius;

    float sphereX = R * cosf((float)u);   // 원 위의 x
    float sphereY = R * sinf((float)u);   // 원 위의 y (= 단면의 높이)

    float dx = 0.0f - sphereX;
    float dy = R   - sphereY;
    float dist = sqrtf(dx*dx + dy*dy);    // top point까지의 거리

    float modulatedR = dist * mFatness;

    float x = modulatedR * sinf((float)v) * cosf((float)u);
    float y = R          * cosf((float)v);
    float z = modulatedR * sinf((float)v) * sinf((float)u);

    return vmath::vec4(x, y, z, 1.0f);
}
```

**문제**: `u`를 **단면 프로파일 결정**과 **3D 회전 경도** 양쪽에 동시에 사용한다. 그래서 단면마다 `dist`가 달라지면서 회전체가 균일하지 않게 비틀려 **커피콩 모양**이 나왔다. 사용자 피드백: *"이건 물방울이 아니라 커피콩 같은데?"*

### Stage B — 책임 분리: PDF의 극형식(equation 1) 적용

```cpp
// PDF 간략 극좌표: r = fatness * sin(v/2),  2D 프로파일 = (R·r·sin v, R·cos v)
// 3D: 프로파일을 u로 Y축 회전
virtual vmath::vec4 SurfaceFunction(double u, double v) const override
{
    float R = mRadius;

    // 프로파일은 v 만, 회전은 u 만
    float r = mFatness * sinf((float)v / 2.0f);
    float horizontalR = R * r * sinf((float)v);  // Y축으로부터 수평 거리

    float x = horizontalR * cosf((float)u);
    float y = R           * cosf((float)v);
    float z = horizontalR * sinf((float)u);

    return vmath::vec4(x, y, z, 1.0f);
}
```

**개선점**: `u`는 경도(회전), `v`는 프로파일로 책임을 명확히 분리. 비틀림이 사라지고 회전 대칭이 회복됨.

**남은 아쉬움**: `sin(v/2) · sin(v)` 는 작동하지만 곡선이 다소 부자연스럽다.

### Stage C — Bourke 공식으로 교체

```cpp
// 물방울 형태
// paulbourke.net/geometry/teardrop + CJ01_WaterShader.pdf 통합
// Bourke: horizontalR = (1 − cos θ) · sin θ,   height = cos θ
// fatness 파라미터로 수평 팽창 정도 제어
// u: 경도 [0, 2π] — Y축 회전, v: 위도 [0, π] — 꼭대기에서 바닥
virtual vmath::vec4 SurfaceFunction(double u, double v) const override
{
    float R = mRadius;
    float theta = (float)v;   // [0, π]  꼭대기 → 바닥
    float phi   = (float)u;   // [0, 2π] Y축 회전

    float horizontalR = R * mFatness * (1.0f - cosf(theta)) * sinf(theta);

    float x = horizontalR * cosf(phi);
    float y = R * cosf(theta);          // 높이 R(꼭대기) → −R(바닥)
    float z = horizontalR * sinf(phi);

    return vmath::vec4(x, y, z, 1.0f);
}
```

**최종형**. Ávalos의 `fatness` 아이디어와 Bourke의 깔끔한 프로파일이 합쳐진 형태. 위 프로파일은:

| θ | `(1−cos θ)·sin θ` | 의미 |
|---|---|---|
| `0` | `0` | 뾰족한 꼭대기 |
| `π/2` | `1` | 수평 팽창 |
| `≈ 2.2` | **최대** (≈ 1.30) | 가장 부풀어오른 부분 |
| `π` | `0` | 바닥 한 점 |

### Stage D — 노멀 안정화 (꼭대기 특이점 잘라내기)

θ=0에서 모든 정점이 `(0, R, 0)` 한 점으로 수렴하면서 삼각형이 퇴화(degenerate)한다. 이 영역에서 노멀 벡터가 불안정해 라이팅이 깨진다.

**해결**: 코드는 그대로 두고, 호출 측에서 `v`의 시작값만 0보다 약간 크게 한다.

```cpp
models.push_back(std::make_unique<WaterDrop>(
    0.0, 2.0 * M_PI, 32,
    0.1,    M_PI, 16,   // ← v_start = 0.0 → 0.1로 (꼭대기 특이점 회피)
    1.0f, 0.5f));
```

`v_start`를 키울수록 꼭대기를 더 많이 잘라낸 형태가 된다. 0.05~0.15 정도가 시각적으로 자연스럽다.

---

## 3. 두 가지 코드 형태 (OOP vs Data-Oriented)

### 3.1 OOP 버전 — `apps/chapter6_extra_1/main.cpp`, `apps/extra1/main.cpp`

`ParametricGeometry`를 상속한 `WaterDrop` 클래스로 구현. `SurfaceFunction(u, v)`을 override한다.

```cpp
class WaterDrop : public ParametricGeometry
{
private:
    float mRadius;
    float mFatness;   // 0에 가까우면 구, 1 근처면 뾰족한 물방울 (Bourke 기본 0.5)

public:
    WaterDrop(double u_start, double u_end, size_t u_res,
              double v_start, double v_end, size_t v_res,
              float radius = 1.0f, float fatness = 0.5f,
              vmath::vec4 _offset = vmath::vec4(0, 0, 0, 0))
        : ParametricGeometry(u_start, u_end, u_res,
                             v_start, v_end, v_res, _offset),
          mRadius(radius), mFatness(fatness)
    {
        build();   // 추상 가상함수 SurfaceFunction()이 준비된 후에 호출
    }

    virtual vmath::vec4 SurfaceFunction(double u, double v) const override
    {
        float R = mRadius;
        float theta = (float)v;
        float phi   = (float)u;
        float horizontalR = R * mFatness * (1.0f - cosf(theta)) * sinf(theta);
        return vmath::vec4(
            horizontalR * cosf(phi),
            R * cosf(theta),
            horizontalR * sinf(phi),
            1.0f);
    }
};
```

특징: `SurfaceFunction`이 `mRadius`, `mFatness`에 자유롭게 접근 가능. 새 도형을 만들려면 새 subclass 작성.

### 3.2 Data-Oriented 버전 — `apps/chapter7/main.cpp`

이후 진행된 리팩토링에서는 클래스 상속 구조 자체를 폐기하고, **전역 함수 포인터**(`std::function<vmath::vec4(double, double)>`)를 `ModelBase`에 주입하는 방식으로 바뀐다.

```cpp
namespace Chapter7::Surfaces
{
using SurfaceFunction = std::function<vmath::vec4(double u, double v)>;

inline vmath::vec4 WaterDrop(double u, double v)
{
    float R = 1.0f;
    float fatness = 0.5f;          // ← 하드코딩, 파라미터화하려면 람다로 감싸야 함
    float theta = (float)v;
    float phi   = (float)u;
    float horizontalR = R * fatness * (1.0f - cosf(theta)) * sinf(theta);
    return vmath::vec4(
        horizontalR * cosf(phi),
        R * cosf(theta),
        horizontalR * sinf(phi),
        1.0f);
}
} // namespace Chapter7::Surfaces

// 사용: 클래스 선언 없이 ModelBase 하나에 함수 포인터 주입
auto drop = std::make_unique<Model::ModelBase>(
    Surfaces::WaterDrop,
    0.0, 2.0 * M_PI, 32,
    0.1, M_PI,        16,
    vmath::vec3{0.0f, 0.0f, 0.0f});
drop->Build();
```

특징: subclass 없이 함수만 추가하면 새 도형 추가 가능. 단점은 멤버 파라미터(`mRadius`, `mFatness`)를 캡처하려면 람다나 `std::bind`가 필요.

**파라미터화 패턴**:

```cpp
auto wd = [R = 1.0f, fatness = 0.5f](double u, double v) -> vmath::vec4 {
    float theta = (float)v, phi = (float)u;
    float hR = R * fatness * (1.0f - cosf(theta)) * sinf(theta);
    return vmath::vec4(hR * cosf(phi), R * cosf(theta), hR * sinf(phi), 1.0f);
};
auto drop = std::make_unique<Model::ModelBase>(wd, 0.0, 2*M_PI, 32, 0.1, M_PI, 16);
```

### 3.3 두 버전의 비교

| 항목 | OOP (extra1) | Data-Oriented (chapter7) |
|---|---|---|
| 주입 방식 | `class WaterDrop : public ParametricGeometry` | `Surfaces::WaterDrop` 전역 함수 |
| 새 도형 추가 | 새 subclass 작성 | 함수 추가 |
| 파라미터 보유 | 멤버 변수 (`mRadius`, `mFatness`) | 람다 캡처 또는 `std::bind` |
| 인스턴스별 다른 fatness | 자연스러움 | 람다로 감싸야 함 |
| 코드량 | 더 많음 | 적음 |

---

## 4. 파라미터 가이드

### 4.1 권장 호출 형태

```cpp
WaterDrop(
    0.0, 2.0 * M_PI, 32,   // u: 경도 [0, 2π], 32분할 → 부드러운 회전
    0.1,        M_PI, 16,   // v: 위도 [0.1, π], 16분할 → 꼭대기 특이점 회피
    1.0f,                   // radius: 1
    0.5f);                  // fatness: 0.5 (Bourke 기본값)
```

### 4.2 각 인자의 의미

| 인자 | 역할 | 권장값 | 너무 작으면 / 너무 크면 |
|---|---|---|---|
| `u_res` (경도 분할) | 회전 방향 정점 수 | 24 ~ 64 | 작으면 다각형처럼 보이고, 크면 정점 수가 제곱으로 증가 |
| `v_res` (위도 분할) | 프로파일 곡선 정점 수 | 12 ~ 24 | 작으면 곡선이 거칠고, 크면 메모리/연산 증가 |
| `v_start` | 꼭대기 잘라내기 | 0.05 ~ 0.15 | 0이면 노멀 깨짐, 너무 크면 위가 평평해짐 |
| `v_end` | 보통 `π` | `π` | 보통 그대로 |
| `radius R` | 전체 크기 (높이/2) | 1.0 | 단순 스케일 |
| `fatness` | 수평 팽창 강도 | 0.3 ~ 0.7 | 0이면 점, 1이면 매우 부풀어오른 형태 |

### 4.3 fatness 별 형태 (Ávalos PDF 기준)

| fatness | 형태 |
|---|---|
| 0.0 | 직선(높이만 남음) |
| 0.5 | **표준 물방울** (PDF Figure 3) |
| 1.0 | 매우 부풀어오른 형태 (PDF Figure 2) |

> 주의: PDF 공식과 Bourke 공식은 정확히 같은 함수가 아니므로 fatness 값의 시각 효과가 미세하게 다를 수 있다. Bourke 공식의 자연스러운 영역은 대략 0.3 ~ 0.7.

---

## 5. 자주 만나는 함정과 해결

### 5.1 커피콩 모양

**증상**: 옆에서 보면 물방울인데 위에서 보면 비대칭으로 비틀려 있음.

**원인**: `u`를 단면 프로파일 결정과 회전 양쪽에 사용. (Stage A에서 발생)

**해결**: `u`는 회전(경도), `v`는 프로파일에만 쓴다. Stage B 이후의 코드 형태를 따른다.

### 5.2 꼭대기에서 라이팅이 깨짐

**증상**: 물방울 끝점 주변에서 셰이딩이 갑자기 어두워지거나 잡티가 생김.

**원인**: `v=0`에서 `horizontalR = 0` → 모든 경도 `u`의 정점이 동일한 점 `(0, R, 0)`으로 수렴 → 삼각형 퇴화 → 노멀 벡터 정의 불가.

**해결**: `v_start = 0.1` 정도로 꼭대기를 살짝 잘라낸다. (Stage D)

### 5.3 회전체가 절반만 그려짐

**증상**: 옆면이 절반만 있고 뒤가 뚫려 있음.

**원인**: `u` 범위를 `[0, π]`로 줬다. 일부 코드 주석에서 잘못 적힌 적이 있음.

**해결**: `u ∈ [0, 2π]`. 회전체이므로 한 바퀴 다 돌려야 한다.

### 5.4 노멀 방향이 안쪽을 향함 (back-face culling으로 안 보임)

**증상**: 모델이 화면에 나타나지 않거나 안쪽 면만 보임.

**원인**: `(u, v)` 격자를 삼각형으로 만들 때 winding이 반대.

**해결**: 인덱스 버퍼 구성에서 CCW를 유지. 정 안 되면 `phi = -u`로 좌우 반전.

---

## 6. 빠른 시작 (요약)

회전체 물방울을 그리고 싶으면 이 한 함수만 알면 된다.

```cpp
vmath::vec4 WaterDrop(double u, double v, float R = 1.0f, float fatness = 0.5f)
{
    float theta = (float)v;
    float phi   = (float)u;
    float hR = R * fatness * (1.0f - cosf(theta)) * sinf(theta);
    return vmath::vec4(hR * cosf(phi), R * cosf(theta), hR * sinf(phi), 1.0f);
}
```

호출:

```cpp
//  u ∈ [0, 2π],  v ∈ [0.1, π],  격자 32×16
auto drop = std::make_unique<WaterDrop>(
    0.0, 2.0 * M_PI, 32,
    0.1,        M_PI, 16,
    1.0f, 0.5f);
```

이게 Ávalos PDF의 `fatness` 개념 + Bourke의 회전체 공식 + 노멀 안정화까지 모두 반영한 최종 형태다.

---

## 7. 친척 도형 — Piriform Surface (Paul Bourke, 2003)

### 7.1 정체와 WaterDrop과의 관계

Paul Bourke가 2003년에 또 하나의 배 모양 회전체를 발표했다. 음함수 형식이 단 한 줄이다.

```
x⁴ − x³ + y² + z² = 0
```

라틴어 *pirum*(배)에서 온 이름이라 진짜 서양 배 모양. WaterDrop과 비슷해 보이지만 **본질적으로 다른 곡선**이다.

| 항목 | WaterDrop (Bourke teardrop) | Piriform Surface (Bourke 2003) |
|---|---|---|
| 출처 | `(1 − cos θ) · sin θ` | `x⁴ − x³ + y² + z² = 0` |
| 정의 형식 | 매개변수(parametric) | 음함수(implicit) → 매개변수로 풀어야 함 |
| 회전축 (원본) | Y축 (수직) | X축 (수평) |
| 양 끝 형태 | 위·아래 모두 점 (대칭) | x=0 뾰족, x=1 둥근 점 (**비대칭**) |
| 끝점 종류 | 양쪽 모두 첨점 + 1차 영점 | 한쪽만 첨점(cusp), 한쪽은 1차 영점 |
| 단면 반경 함수 | `(1−cos θ)·sin θ` | `√(t³(1−t))` |
| 최대 반경 위치 | θ ≈ 2.2 rad | t = 0.75 (비대칭) |

WaterDrop은 위아래가 대칭인 진짜 눈물방울이고, Piriform은 한쪽만 뾰족한 서양 배 — `(x³)(1−x)`의 부호 영역이 `x ∈ [0, 1]`로 닫혀 있어서 **자연스럽게 한쪽만 뾰족**해진다. 일반화 형태 `(x⁴ − a·x³) + a²(y² + z²) = 0`으로 쓰면 `a` 파라미터로 비례를 조절할 수 있다 (Bourke 식은 `a = 1`인 특수 경우).

### 7.2 Piriform 매개변수화

음함수를 풀면 단면 반경이 닫힌 형태로 나온다.

```
y² + z² = x³ − x⁴ = x³(1 − x)     (x ∈ [0, 1])
∴ r(x) = √(x³(1 − x))
```

이걸 회전체로 풀어서 X축 회전체 매개변수화를 얻는다.

```cpp
// X축 회전체 (Bourke 원형식)
inline vmath::vec4 PiriformX(double t, double v)
{
    float r = sqrtf((float)((t*t*t) * (1.0 - t)));
    return vmath::vec4(
        (float)t,           // x : [0, 1]
        r * cosf((float)v), // y
        r * sinf((float)v), // z
        1.0f);
}
// 호출 : t ∈ [0.05, 0.98], v ∈ [0, 2π]
//        ↑ 양쪽 끝 둘 다 잘라야 함 (이유는 7.4 참조)
```

Y축 회전체 버전(Piriform을 WaterDrop처럼 세워서 쓰고 싶을 때):

```cpp
// Y축 회전체 (Piriform을 세움)
inline vmath::vec4 PiriformY(double t, double v)
{
    float r = sqrtf((float)((t*t*t) * (1.0 - t)));
    return vmath::vec4(
        r * cosf((float)v), // x
        (float)t,           // y : [0, 1]  (회전축이 Y)
        r * sinf((float)v), // z
        1.0f);
}
```

### 7.3 GeoGebra에서 그리기

음함수 `x⁴ − x³ + y² + z² = 0`을 GeoGebra 3D에 직접 입력하면 거의 안 그려진다 — GeoGebra의 **음함수 곡면 렌더링이 매우 빈약**하기 때문이다. 반드시 `Surface(...)` 명령으로 매개변수 형태를 줘야 한다.

**복붙용 (3D Calculator 또는 Classic 6 입력창)**:

```
Surface(t, sqrt(t^3 (1 - t)) cos(v), sqrt(t^3 (1 - t)) sin(v), t, 0, 1, v, 0, 2pi)
```

안 보일 때 체크리스트:
- 그래픽 뷰 우클릭 → **Show All Objects** (도형이 `x ∈ [0,1]`로 작아서 화면 밖에 있을 수 있음)
- 곱셈은 공백 또는 `*` 명시 (GeoGebra가 한국어 로케일에서 `cos(u)(1-sin(u))` 류 표기를 함수 호출로 잘못 해석할 수 있음)
- GeoGebra 6 최신 버전 권장 — 구버전은 `Surface()` 인자 처리가 다름

### 7.4 Y축 회전체로 만들 때의 아티팩트

Piriform을 WaterDrop처럼 Y축 회전체로 정의하면 (즉 `PiriformY` 형태로) 다음 아티팩트들이 발생한다. 자주 발생하는 순서대로.

#### A. Cusp 노멀 폭주 (가장 흔하고 두드러짐)

뾰족한 끝(`t = 0`)은 단순한 sphere의 극점이 아니라 진짜 **첨점(cusp)**. 곡면이 한 점에 모이면서 **접평면이 정의되지 않는 특이점**이다.

수식으로:

```
r(t) = √(t³(1−t))
dr/dt = (3t² − 4t³) / (2√(t³(1−t)))
t → 0⁺ 일 때 r ~ t^(3/2) → 0  (1.5차 영점)
```

**WaterDrop의 끝보다 더 뾰족하다.** WaterDrop은 단면 반경이 `(1−cos θ)·sin θ ~ θ³/2` (3차 영점)이고, piriform은 `t^(3/2)` (1.5차 영점). 차수가 낮을수록 끝이 더 첨예하고 노멀이 더 불안정하다.

**증상**: 뾰족한 끝 주변이 반짝거림(specular speckle), 얼룩진 어두운 점, 텍스처 뒤틀림.

**처방**: WaterDrop의 `v_start` 처방과 동일하지만 **더 많이 잘라야 한다**. `t ∈ [0.05, ...]` 정도. WaterDrop의 0.1보다 안전 마진을 더 줘야 한다.

#### B. 둥근 끝의 degenerate triangle

`t = 1`에서도 `r = 0`이라 또 한 번 한 점으로 수렴. 여기는 cusp는 아니고 (접평면은 잘 정의됨, 회전축에 수직) sphere의 극점과 같은 종류 — **모든 경도 정점이 한 점에 모여 퇴화 삼각형**이 된다.

**증상**: 둥근 끝부분이 약간 어둡거나 라이팅이 한 점에서 터짐. backface culling 켜져 있으면 깜빡임.

**처방**: `t ∈ [..., 0.98]`로 살짝만 잘라도 충분하다. 1차 영점이라 cusp만큼 심각하진 않음.

#### C. 비대칭 단면 분포로 인한 UV/메시 왜곡

WaterDrop의 단면 함수는 [0, π]에서 거의 대칭이지만, piriform의 `√(t³(1−t))`는 **최대점이 t = 0.75에 치우쳐** 있다. 따라서 `t`를 균등 분할하면:

- 가장 부풀어오른 부분(0.5~0.9)에 정점이 부족 → 약간 각진 형태
- 뾰족한 끝(0~0.3) 쪽에 정점이 낭비
- 텍스처 매핑이 불균등해짐 (UV 균등 분포라면 뚱뚱한 부분 텍스처 늘어남)

**처방 1**: 적응적 reparameterization. `t = 0.5·(1 − cos s)` 로 두고 `s`를 균등 분할하면 양 끝에 정점이 더 모인다 (Chebyshev 분포 비슷).

**처방 2**: 그냥 `v_res`를 크게(24~32) 주고 무시. 강의 과제용이라면 보통 이걸로 충분.

#### D. Winding 반전 가능성

Y축 회전체 버전은 양 끝 형태가 비대칭(한쪽 cusp, 한쪽 점)이라 (u, v) 격자를 같은 인덱스 규칙으로 삼각형화하면 한쪽 끝에서 outward normal이 나오고 반대쪽 끝에서 **inward로 뒤집힐 가능성**이 있다. 특히 cusp 근처.

**확인**: `glDisable(GL_CULL_FACE)`로 culling을 끄고 양면 다 그려보기. 사라졌던 부분이 다시 나타나면 winding 문제 확정.

**처방**: 인덱스 버퍼 winding 일관되게 유지. 안 되면 `phi = -v` (또는 인덱스 순서 swap)로 반전.

### 7.5 처방 우선순위 (요약)

| 우선 | 처방 | 효과 |
|---|---|---|
| 1순위 | `t ∈ [0.05, 0.98]` 양 끝 잘라내기 | cusp 노멀 폭주 + degenerate 동시 해결 |
| 2순위 | 노멀을 finite difference 대신 **해석적**으로 계산 | cusp 주변 셰이딩 매끈해짐 |
| 3순위 | `t = 0.5·(1 − cos s)` reparameterization | 메시 균일성 개선, 정점 분포 최적화 |
| 4순위 | culling 끄고 winding 검증 | inward normal 확인 |

### 7.6 한 줄 요약

> Piriform은 **WaterDrop의 사촌**(둘 다 Bourke 출처) — 단, 양 끝이 비대칭이고 뾰족한 끝이 더 첨예해서 **WaterDrop 처방을 양쪽 끝 모두에 더 강하게 적용**해야 한다.

---

## 부록 A — 참고 자료

- Mariana Ávalos Arce. *Code journal: water droplet model* (2019). Wolfram Mathematica 11.3 기반의 2D 도출 과정.
- Paul Bourke. *Teardrop*. https://paulbourke.net/geometry/teardrop/ — 회전체 closed-form. (WaterDrop)
- Paul Bourke. *Piriform tear drop* (2003). `x⁴ − x³ + y² + z² = 0`. (7장)
- MathWorld. *Piriform Curve / Piriform Surface*. 일반화 형식 `(x⁴ − a·x³) + a²(y² + z²) = 0`.
- 본 정리집의 1차 자료: 두 jsonl 세션 로그 (`b8a9ada8-...` Stage A→D 진화, `c321bf36-...` Data-Oriented 리팩토링).

## 부록 B — 코드 한눈에 보기 (Bourke 최종형)

```cpp
class WaterDrop : public ParametricGeometry
{
    float mRadius;
    float mFatness;
public:
    WaterDrop(double u_start, double u_end, size_t u_res,
              double v_start, double v_end, size_t v_res,
              float radius = 1.0f, float fatness = 0.5f,
              vmath::vec4 _offset = vmath::vec4(0,0,0,0))
        : ParametricGeometry(u_start, u_end, u_res, v_start, v_end, v_res, _offset),
          mRadius(radius), mFatness(fatness) { build(); }

    virtual vmath::vec4 SurfaceFunction(double u, double v) const override
    {
        float R = mRadius;
        float theta = (float)v;
        float phi   = (float)u;
        float hR = R * mFatness * (1.0f - cosf(theta)) * sinf(theta);
        return vmath::vec4(hR * cosf(phi),
                           R  * cosf(theta),
                           hR * sinf(phi),
                           1.0f);
    }
};
```

## 부록 C — Piriform Y축 회전체 (7장 최종형)

```cpp
class PiriformY : public ParametricGeometry
{
    // 회전축은 Y. t는 단면 위치, v는 경도.
    // r(t) = √(t³(1−t)),  t ∈ [0.05, 0.98] 권장 (양 끝 cusp/degenerate 회피)
public:
    PiriformY(double t_start, double t_end, size_t t_res,
              double v_start, double v_end, size_t v_res,
              vmath::vec4 _offset = vmath::vec4(0,0,0,0))
        : ParametricGeometry(t_start, t_end, t_res, v_start, v_end, v_res, _offset)
    { build(); }

    virtual vmath::vec4 SurfaceFunction(double u, double v) const override
    {
        // u 를 t 로 사용 (회전축 위치)
        float t = (float)u;
        float r = sqrtf(t * t * t * (1.0f - t));   // = √(t³(1−t))
        float phi = (float)v;
        return vmath::vec4(r * cosf(phi),
                           t,                       // y = t (회전축)
                           r * sinf(phi),
                           1.0f);
    }
};

// 사용 예
auto pir = std::make_unique<PiriformY>(
    0.05, 0.98, 24,        // t : 양 끝 잘라냄
    0.0,  2.0 * M_PI, 32); // v : 경도 한 바퀴
```
