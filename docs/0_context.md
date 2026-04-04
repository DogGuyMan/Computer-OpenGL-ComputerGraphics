
---

## Context란 무엇인가

---

Context는 **"어떤 작업이 실행되는 환경/상태 정보"** 이다. DTO가 아니다.

```
DTO:     "이 값들 저쪽에 전달해줘"      -> 수동적, 데이터 컨테이너
Context: "이 환경 안에서 실행해"          -> 능동적, 실행 환경
```

> ### 📄 3가지 Context가 같은 개념인 이유

#### 1). OpenGL Context — GPU가 그리기 위한 실행 환경

`glutInit` -> `glutCreateWindow` 시점에 OS가 윈도우 + OpenGL Context를 생성한다.
이 Context 안에 렌더링 상태(현재 셰이더, 텍스처, 뷰포트 등)가 전부 담겨 있고,
모든 `gl*` 호출은 **현재 바인딩된 Context 위에서** 실행된다.

```
glutCreateWindow() -> OS가 OpenGL Context 생성 -> gl* 호출의 실행 환경이 됨
```

#### 2). 매개변수 묶음으로서의 Context — 함수 실행에 필요한 환경

```cpp
// 매개변수가 흩어져 있으면
void render(Camera* cam, Light* light, Scene* scene, Viewport vp);

// 실행 환경을 하나로 묶으면
void render(RenderContext& ctx);
```

이것도 "render가 실행되는 환경"을 하나로 묶은 것이다.

#### 3). AppContext — GLUT 콜백이 실행되는 환경

GLUT 콜백은 `void(*)(void)` 같은 시그니처라서 매개변수를 받을 수 없다.
그런데 콜백 안에서 Camera, Display, Input에 접근해야 한다.

```cpp
// 콜백의 실행 환경을 전역으로 제공
static AppContext g_ctx;

void HandleDisplayEvent() {
    // 이 함수가 "어떤 환경에서" 실행되는지 g_ctx가 정의
    g_ctx.display.Render(g_ctx.camera);
}
```

### 왜 이 프로젝트에서 Context가 적합했는가

GLUT의 제약 때문이다:

```
GLUT 콜백 시그니처가 고정 -> 매개변수로 환경을 전달할 수 없음
-> 그러면 실행 환경을 어딘가에 놓아야 함
-> 전역 Context가 가장 자연스러운 해법
```

만약 콜백에 `void*` userdata를 받을 수 있었다면
(GLFW의 `glfwSetWindowUserPointer`처럼) 전역이 아닌 포인터로 넘길 수 있었겠지만,
GLUT은 그 설계가 아니다. GLUT 자체가 전역 상태 기반이라 전역 Context와 궁합이 맞다.
