# freeglut + OpenGL용 UI 오버레이 라이브러리 완벽 비교

**Dear ImGui가 압도적 1순위다.** freeglut + 레거시 OpenGL 환경에서 3D 뷰포트 위에 파라미터 조절 UI를 오버레이하려면, Dear ImGui의 공식 GLUT 백엔드(`imgui_impl_glut` + `imgui_impl_opengl2`)가 위젯 풍부함, 유지보수 상태, 통합 난이도, 크로스플랫폼 지원 모든 면에서 최적이다. **GitHub 스타 ~72k, 2026년 2월 v1.92.6 릴리스**로 활발히 유지되며, FBO 없이 기존 3D 씬 위에 바로 오버레이 렌더링이 가능하다. AntTweakBar와 GLUI는 각각 2016년, 2021년 이후 사실상 방치 상태여서 신규 프로젝트에는 권장하지 않는다.

---

## 네 가지 후보 라이브러리의 현재 상태

### Dear ImGui — 사실상 업계 표준 즉시 모드 GUI

| 항목 | 내용 |
|------|------|
| GitHub | [ocornut/imgui](https://github.com/ocornut/imgui) — **~72k stars**, 11.6k forks |
| 최신 버전 | v1.92.6 (2026-02-17 릴리스) |
| 라이선스 | MIT |
| GLUT 백엔드 | `backends/imgui_impl_glut.cpp/.h` 공식 제공, freeglut 명시 지원 |
| 오버레이 방식 | **별도 FBO 불필요** — 동일 프레임버퍼에서 3D 씬 뒤에 그리고 ImGui를 위에 렌더 |
| 크로스플랫폼 | macOS ✅ Windows ✅ Linux ✅ |

Dear ImGui의 GLUT 백엔드는 원래 `imgui_impl_freeglut`이라는 이름이었고(2019년 개명), 내부적으로 macOS에서는 `<GLUT/glut.h>`, 그 외 플랫폼에서는 `<GL/freeglut.h>`를 직접 include한다. 소스코드에 "GLUT IS OBSOLETE PREHISTORIC SOFTWARE"라는 경고 주석이 있지만 이는 GLFW/SDL 전환을 권장하는 것일 뿐, **백엔드 기능 자체는 정상 작동**한다. 공식 예제 `examples/example_glut_opengl2/`가 165줄짜리 완전한 참조 구현을 제공한다.

**위젯 지원이 가장 풍부하다.** `SliderFloat/2/3/4`, `DragFloat/2/3/4`, `DragFloatRange2`, `InputFloat/2/3/4`, `ColorEdit3/4`, `ColorPicker3/4`, `Checkbox`, `Combo`, `TreeNode`, `CollapsingHeader`, `TabBar` 등 파라미터 조절에 필요한 거의 모든 위젯을 기본 제공한다. 4×4 행렬 전용 위젯은 없지만 `InputFloat4` 네 줄이면 충분하다. 또한 모든 Drag/Slider 위젯에서 **Ctrl+클릭으로 키보드 직접 입력**이 가능하다.

### AntTweakBar — 우수한 위젯이지만 사실상 폐기

| 항목 | 내용 |
|------|------|
| GitHub | [tschw/AntTweakBar](https://github.com/tschw/AntTweakBar) — 43 stars (커뮤니티 포크) |
| 최종 유지보수 | **~2016년 이후 방치**, 원저자 Philippe Decaudin이 더 이상 유지보수하지 않음을 명시 |
| GLUT 지원 | ✅ `TwEventMouseButtonGLUT` 등 내장 GLUT 이벤트 핸들러 |
| 특이 위젯 | **쿼터니언 회전 구체**(TW_TYPE_QUAT4F), 방향 벡터 위젯 |
| 크로스플랫폼 | Windows/Linux는 가능, macOS는 포크 패치 필요 |

AntTweakBar의 쿼터니언 회전 위젯과 방향 벡터 위젯은 3D 애플리케이션에 매력적이지만, **2016년 이후 유지보수가 중단**되어 최신 컴파일러나 macOS에서 빌드 문제가 발생할 수 있다. 여러 포크가 난립해 있으나 어느 것도 활발히 관리되지 않는다.

### GLUI — GLUT 전용 설계지만 시대에 뒤처짐

| 항목 | 내용 |
|------|------|
| GitHub | [libglui/glui](https://github.com/libglui/glui) — 213 stars |
| 최종 유지보수 | **2021년 8월 이후 업데이트 없음**, 64개 미해결 이슈 |
| GLUT 지원 | ✅ GLUT 위에 직접 구축된 라이브러리 — 최고의 호환성 |
| 위젯 | Spinner, Rotation/Translation 컨트롤러, Checkbox, Rollout 등 |
| 외관 | **Windows 95 스타일** — 시각적으로 매우 구식 |

GLUI는 GLUT 위에 직접 구축되어 통합 마찰이 거의 없다는 장점이 있다. 하지만 **슬라이더 위젯이 기본 제공되지 않고**(Spinner/Scrollbar로 대체), UI 외관이 극히 구식이며, 위젯 종류가 ImGui에 비해 현저히 부족하다.

### 기타 대안 — Nuklear, NanoGUI, nfd

**Nuklear**(~10.6k stars, 활발히 유지)은 ANSI C 단일 헤더 즉시 모드 UI로 위젯이 풍부하지만, **GLUT 백엔드가 없어** 직접 작성해야 한다. 수업 과제에는 과도한 작업량이다. **NanoGUI**는 OpenGL 3.x + GLFW를 요구하므로 레거시 OpenGL + freeglut 환경에서는 사용 불가. **nfd(Native File Dialog)**는 파일 열기/저장 전용이며 파라미터 조절 UI와는 무관하다.

---

## ImGui GLUT 백엔드 통합의 핵심 기술 사항

### 렌더링 파이프라인과 오버레이 원리

ImGui의 3D 뷰포트 오버레이는 매우 단순한 원리로 작동한다. **같은 기본 프레임버퍼에서 3D 씬을 먼저 그리고, 그 위에 ImGui의 2D UI를 알파 블렌딩으로 덮어 그린다.** `imgui_impl_opengl2`가 내부적으로 깊이 테스트 비활성화, 시저 테스트 활성화, 블렌딩 활성화를 처리하고, 기존 OpenGL 상태(프로젝션/모델뷰 행렬, 블렌드 모드, 텍스처 등)를 백업/복원하므로 3D 씬 상태가 오염되지 않는다.

렌더 루프의 정확한 호출 순서는 다음과 같다:

```cpp
void DisplayFunc() {
    // 1. ImGui 프레임 시작
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();

    // 2. ImGui 위젯 정의 (파라미터 변경은 여기서 즉시 반영)
    ImGui::Begin("Surface Parameters");
    ImGui::SliderFloat("U max", &u_max, 0.0f, 6.28f);
    ImGui::End();

    // 3. ImGui 렌더 커맨드 생성
    ImGui::Render();

    // 4. 3D 씬 렌더링
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawParametricSurface(u_max);  // 위에서 변경된 파라미터가 즉시 적용

    // 5. ImGui 오버레이를 3D 씬 위에 렌더링
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    glutSwapBuffers();
    glutPostRedisplay();
}
```

핵심은 **4단계(3D 씬)와 5단계(ImGui 드로우) 사이의 순서**다. `ImGui::Render()`는 드로우 리스트만 생성하고 실제 GPU 그리기는 `RenderDrawData`에서 수행되므로, 3D 씬 렌더링 코드를 그 사이에 삽입하면 자연스럽게 오버레이가 된다.

### GLUT 콜백 충돌 해결 — 수동 포워딩 패턴

GLUT의 근본적 한계는 **콜백 타입당 하나만 등록 가능**하다는 것이다. `ImGui_ImplGLUT_InstallFuncs()`를 호출하면 기존 콜백을 덮어쓴다. 3D 씬에서 마우스 카메라 회전 등을 처리하려면, **`InstallFuncs()`를 호출하지 말고** 직접 래퍼 콜백을 작성해야 한다:

```cpp
void MyMouseFunc(int button, int state, int x, int y) {
    // 항상 ImGui에 먼저 전달
    ImGui_ImplGLUT_MouseFunc(button, state, x, y);
    
    // ImGui가 마우스를 캡처하지 않을 때만 3D 씬 처리
    if (!ImGui::GetIO().WantCaptureMouse) {
        Camera_HandleMouse(button, state, x, y);
    }
}

void MyKeyboardFunc(unsigned char c, int x, int y) {
    ImGui_ImplGLUT_KeyboardFunc(c, x, y);
    if (!ImGui::GetIO().WantCaptureKeyboard) {
        Scene_HandleKey(c, x, y);
    }
}

// 등록 시:
glutMouseFunc(MyMouseFunc);
glutMotionFunc(MyMotionFunc);       // 동일 패턴
glutPassiveMotionFunc(MyMotionFunc);
glutKeyboardFunc(MyKeyboardFunc);
glutReshapeFunc(MyReshapeFunc);
// ... 나머지 콜백도 동일 패턴
```

**`io.WantCaptureMouse`가 `true`이면** 마우스가 ImGui 위젯 위에 있다는 뜻이므로, 3D 카메라 회전을 차단해야 한다. 이 플래그는 `ImGui::NewFrame()` 시점에 이전 프레임 상태 기준으로 업데이트된다.

### OpenGL2 vs OpenGL3+ 백엔드 선택

**freeglut + 레거시 OpenGL 프로젝트에서는 반드시 `imgui_impl_opengl2`를 사용한다.** 이 백엔드는 `glBegin/glEnd`, `glVertexPointer`, `glOrtho` 등 고정 파이프라인 함수만 사용하므로 셰이더 없는 환경과 완벽히 호환된다. `imgui_impl_opengl3`는 커스텀 셰이더와 VBO/VAO를 요구하므로 레거시 GL 컨텍스트에서는 동작하지 않는다.

OpenGL2 백엔드의 알려진 제한사항은 **64k+ 정점 메시 미지원**(16비트 인덱스 한계)과 WebGL 미지원이지만, 과제 수준의 UI에서는 전혀 문제되지 않는다.

---

## Parametric Surface + Affine Transform에 최적화된 ImGui 위젯 구성

ImGui의 즉시 모드(Immediate Mode) 특성상, 위젯 값이 변경되면 **같은 프레임에서 즉시 반영**된다. 옵저버 패턴이나 콜백 등록 없이, `SliderFloat`가 `true`를 반환하면 변수가 이미 업데이트된 상태이므로 바로 렌더링에 사용하면 된다.

### u/v 파라미터와 서브디비전 제어

```cpp
// 파라미터 범위
static float u_range[2] = {0.0f, 6.28f};
static float v_range[2] = {0.0f, 3.14f};
static int subdivisions[2] = {50, 50};

ImGui::DragFloatRange2("U range", &u_range[0], &u_range[1],
                        0.01f, 0.0f, 6.28f, "Min: %.2f", "Max: %.2f");
ImGui::DragFloatRange2("V range", &v_range[0], &v_range[1],
                        0.01f, 0.0f, 3.14f, "Min: %.2f", "Max: %.2f");
ImGui::SliderInt2("Subdivisions (U/V)", subdivisions, 2, 200);
```

`DragFloatRange2`는 min/max를 하나의 위젯으로 조절할 수 있어 u/v 범위 제어에 이상적이다.

### 4×4 아핀 변환 행렬 입력

```cpp
static float matrix[16] = {
    1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1
};

ImGui::Text("Affine Transform Matrix:");
bool changed = false;
changed |= ImGui::DragFloat4("Row 0", &matrix[0],  0.01f);
changed |= ImGui::DragFloat4("Row 1", &matrix[4],  0.01f);
changed |= ImGui::DragFloat4("Row 2", &matrix[8],  0.01f);
changed |= ImGui::DragFloat4("Row 3", &matrix[12], 0.01f);

if (changed) glLoadMatrixf(matrix);  // 즉시 적용
```

또는 **분해된 파라미터**(Translation/Rotation/Scale)로 편집 후 행렬을 재구성하는 방식이 더 직관적일 수 있다:

```cpp
static float translation[3] = {0, 0, 0};
static float rotation[3] = {0, 0, 0};     // 오일러 각(도)
static float scale[3] = {1, 1, 1};

ImGui::DragFloat3("Translation", translation, 0.1f);
ImGui::DragFloat3("Rotation (deg)", rotation, 1.0f, -360.0f, 360.0f);
ImGui::DragFloat3("Scale", scale, 0.01f, 0.01f, 10.0f);
```

3D 기즈모가 필요한 경우 **ImGuizmo**(github.com/CedricGuillemet/ImGuizmo)를 추가하면 뷰포트에서 직접 Translate/Rotate/Scale 핸들을 조작할 수 있다. ImGui 위에 구축된 헤더 온리 라이브러리로 통합이 간단하다.

---

## CMake 통합 — 최소 코드로 빌드 구성

Dear ImGui는 **의도적으로 공식 CMakeLists.txt를 제공하지 않는다.** 저자 ocornut의 방침이며, 직접 소스 파일을 프로젝트에 추가하는 방식을 권장한다. 아래는 가장 실용적인 두 가지 접근법이다.

### 방법 1: Git Submodule + 직접 소스 추가 (권장)

```cmake
cmake_minimum_required(VERSION 3.14)
project(ParametricSurfaceViewer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

# ── 시스템 패키지 ──
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# ── ImGui (vendor/imgui에 git submodule로 추가) ──
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)
add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glut.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl2.cpp
)
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)
target_link_libraries(imgui PUBLIC OpenGL::GL GLUT::GLUT)

# ── 애플리케이션 ──
add_executable(viewer main.cpp)
target_link_libraries(viewer PRIVATE imgui)
```

### 방법 2: FetchContent로 자동 다운로드

```cmake
include(FetchContent)
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.92.6
)
FetchContent_MakeAvailable(imgui)

# 이후 ${imgui_SOURCE_DIR}를 IMGUI_DIR로 사용하여 위와 동일하게 구성
```

macOS에서는 시스템 GLUT 프레임워크가 사용되고, Windows에서는 `vcpkg install freeglut --triplet=x64-windows` 후 `find_package(GLUT)`로 찾을 수 있다. Linux는 `apt install freeglut3-dev`면 충분하다.

---

## 종합 비교와 최종 추천

| 평가 기준 | Dear ImGui | AntTweakBar | GLUI | Nuklear |
|-----------|-----------|-------------|------|---------|
| GLUT 호환 | ✅ 공식 백엔드 | ✅ 내장 핸들러 | ✅ GLUT 기반 설계 | ❌ 백엔드 없음 |
| 레거시 GL | ✅ OpenGL2 백엔드 | ✅ | ✅ | ✅ (GLFW 전용) |
| 유지보수 | ✅ 2026년 활발 | ❌ 2016년 중단 | ⚠️ 2021년 중단 | ✅ 활발 |
| 위젯 풍부함 | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐ |
| 통합 난이도 | **~20줄 보일러플레이트** | 중간 | 매우 쉬움 | 높음 (커스텀 백엔드) |
| 크로스플랫폼 | ✅ | ⚠️ macOS 패치 필요 | ✅ | ✅ |
| UI 외관 | 현대적, 커스터마이즈 가능 | 깔끔함 | 매우 구식(90년대) | 커스터마이즈 가능 |

**1순위: Dear ImGui.** 위젯이 가장 풍부하고, 활발히 유지되며, 공식 GLUT + OpenGL2 예제가 있어 과제 통합이 가장 수월하다. 즉시 모드 특성상 파라미터 변경이 같은 프레임에 반영되어, Parametric Surface의 u/v 범위나 아핀 변환 행렬을 슬라이더로 실시간 조작하기에 최적이다. 필요한 파일을 프로젝트에 복사하고 CMake에 7개 소스 파일을 추가하면 끝이다.

2순위로 GLUI를 고려할 수 있으나, 위젯 부족(슬라이더 미제공)과 구식 외관이 단점이다. AntTweakBar는 쿼터니언 위젯이 매력적이지만 유지보수 중단 리스크가 크다.