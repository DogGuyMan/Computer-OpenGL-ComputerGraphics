# OpenGL-ComputerGraphics {#mainpage}

컴퓨터그래픽스및AI응용 과목용 C++17 / CMake OpenGL 학습 프로젝트의 API 문서입니다.
좌측 트리에서 `Metahuman::` 네임스페이스의 공용 모듈(camera, display, model,
renderer, inputs, user_interface 등)을 탐색할 수 있습니다.

- 소스: `src/` (공용 모듈, `project_srcs` 정적 라이브러리)
- 실행 타겟: `apps/` (기본) · `tutorials/` · `samples/` (배타적)
- 의존성: FetchContent (glm, spdlog, stb, imgui, imguizmo) — **vcpkg 미사용**

---

# 빌드 시스템 {#build-system}

## 한 줄 요약

C++17, CMake 3.14+, **CMakePresets 기반**, FetchContent 의존성 해결,
크로스 플랫폼(macOS / Linux = Ninja, Windows = MSVC). vcpkg 는 이식성을 위해 사용하지 않습니다.

## include() 순서 (루트 `CMakeLists.txt`)

\dot
digraph IncludeOrder {
  rankdir=TB;
  bgcolor="transparent";
  node [shape=box, style="filled,rounded", fontname="Helvetica", fontsize=10, color="#90a4ae", fillcolor="#eceff1"];
  edge [color="#607d8b", arrowsize=0.8];
  cxx  [label="cmake/CXXStandard.cmake\nC++17 · -Werror · compile_commands", fillcolor="#e3f2fd", color="#1565c0"];
  doxy [label="cmake/Doxygen.cmake\nsjhopengl_setup_doxygen 정의",          fillcolor="#fff9c4", color="#f9a825"];
  dep  [label="cmake/Dependency.cmake\nsystem GL + FetchContent → project_deps", fillcolor="#fff9c4", color="#f9a825"];
  cfg  [label="add_subdirectory(config)\nconfigure_file → config.hpp",      fillcolor="#c8e6c9", color="#2e7d32"];
  src  [label="add_subdirectory(src)\nproject_srcs STATIC",                fillcolor="#bbdefb", color="#1565c0"];
  tgt  [label="apps / tutorials / samples\n배타적 분기",                    fillcolor="#ffe0b2", color="#e65100"];
  call [label="sjhopengl_setup_doxygen()\nSJH_OPENGL_BUILD_DOCS=ON",        fillcolor="#fff9c4", color="#f9a825"];
  cxx -> doxy -> dep -> cfg -> src -> tgt -> call;
}
\enddot

`config/` 가 `add_subdirectory(apps)` 보다 먼저 처리되어야 합니다 —
`configure_file(config.hpp.in config.hpp)` 가 윈도우/리소스 설정 변수에 의존하기 때문입니다.
`doxygen` 타겟은 `project()` 이후 어느 시점이든 등록 가능하므로 마지막에 호출합니다.

## 타겟 의존성 그래프 {#target-deps}

`apps/`·`src/`·`config/` 의 `CMakeLists.txt` 와 `cmake/Dependency.cmake`,
그리고 구성 시점에 `CMakeCache.txt` 로 해결된 의존성 버전을 바탕으로 그린
`target_link_libraries` 그래프입니다. **화살표 A → B 는 "A 가 B 를 링크(의존)"** 를 뜻합니다.

\dot
digraph CMakeTargetDeps {
  rankdir=TB;
  compound=true;
  bgcolor="transparent";
  node [fontname="Helvetica", fontsize=10, shape=box, style="filled,rounded", color="#90a4ae", penwidth=1.2];
  edge [fontname="Helvetica", fontsize=8, color="#607d8b", arrowsize=0.8];

  app [label="OpenGL-ComputerGraphics\nexecutable · apps/main.cpp", fillcolor="#ffe0b2", color="#e65100"];

  subgraph cluster_internal {
    label="프로젝트 내부 타겟"; labeljust="l"; fontname="Helvetica"; fontsize=10;
    style="rounded,filled"; fillcolor="#eceff1"; color="#b0bec5";
    srcs   [label="project_srcs\nSTATIC · src/*.cpp",       fillcolor="#bbdefb", color="#1565c0"];
    config [label="project_config\nINTERFACE · config.hpp", fillcolor="#c8e6c9", color="#2e7d32"];
    deps   [label="project_deps\nINTERFACE 집합 타겟",       fillcolor="#c8e6c9", color="#2e7d32"];
  }

  subgraph cluster_imgui {
    label="ImGui 스택 (수동 STATIC 빌드)"; labeljust="l"; fontname="Helvetica"; fontsize=10;
    style="rounded,filled"; fillcolor="#f3e5f5"; color="#ce93d8";
    imgui    [label="imgui\nSTATIC · docking",  fillcolor="#e1bee7", color="#6a1b9a"];
    imguizmo [label="imguizmo\nSTATIC · master", fillcolor="#e1bee7", color="#6a1b9a"];
  }

  subgraph cluster_fetch {
    label="FetchContent 의존성"; labeljust="l"; fontname="Helvetica"; fontsize=10;
    style="rounded,filled"; fillcolor="#fffde7"; color="#ffe082";
    glm    [label="glm::glm\n1.0.1",            fillcolor="#fff9c4", color="#f9a825"];
    spdlog [label="spdlog::spdlog\nv1.15.1",     fillcolor="#fff9c4", color="#f9a825"];
    stb    [label="stb\nINTERFACE · master",     fillcolor="#fff9c4", color="#f9a825"];
  }

  subgraph cluster_system {
    label="시스템 (find_package)"; labeljust="l"; fontname="Helvetica"; fontsize=10;
    style="rounded,filled"; fillcolor="#eceff1"; color="#b0bec5";
    gl   [label="OpenGL::GL / OpenGL::GLU", fillcolor="#cfd8dc", color="#37474f"];
    glut [label="GLUT::GLUT\n(Win: freeglut_local)", fillcolor="#cfd8dc", color="#37474f"];
  }

  // A -> B : A 가 B 를 target_link_libraries
  app -> srcs   [label="PRIVATE", color="#e65100", fontcolor="#e65100"];
  app -> deps   [style=dashed, label="직접(중복)"];
  app -> config [style=dashed];

  srcs -> deps   [label="PUBLIC"];
  srcs -> config [label="PUBLIC"];

  deps -> glm;
  deps -> spdlog;
  deps -> stb;
  deps -> imgui;
  deps -> imguizmo;
  deps -> gl;
  deps -> glut;

  imguizmo -> imgui [label="PUBLIC"];
  imgui -> gl;
  imgui -> glut;
}
\enddot

점선(`app → project_deps`, `app → project_config`)은 `apps/CMakeLists.txt` 에 실제로
적힌 직접 링크지만, `project_srcs` 가 두 타겟을 **PUBLIC** 으로 재전파하므로 사실상
중복입니다. `project_deps` 는 INTERFACE 집합 타겟으로, 시스템(OpenGL/GLUT)·
FetchContent(glm/spdlog/stb)·ImGui 스택(imgui/imguizmo)을 한곳에 모아 전달합니다.
Windows 에서는 `GLUT::GLUT` 대신 `extern/freeglut` 의 `freeglut_local` 과 `gdi32`·`winmm` 이 추가됩니다.

## project_srcs 내부 모듈 그래프 {#srcs-modules}

`project_srcs` (STATIC) 를 이루는 `src/` 모듈들의 `#include` 관계입니다.
**화살표 A → B 는 "A 가 B 의 헤더를 include (의존)"** 를 뜻하며, 각 모듈의
`.h` · `.cpp` · `_imp.h` 를 하나로 묶어 표시했습니다. 시스템 · 외부 헤더
(OpenGL · glm · imgui · stb · `config.hpp`) 는 생략했습니다 — 이들은 라이브러리
차원에서 `project_deps` · `project_config` 로 일괄 링크됩니다
([타겟 의존성 그래프](#target-deps) 참고).

\dot
digraph SrcModules {
  rankdir=TB;
  compound=true;
  bgcolor="transparent";
  node [fontname="Helvetica", fontsize=10, shape=box, style="filled,rounded", color="#90a4ae", penwidth=1.2];
  edge [fontname="Helvetica", fontsize=8, color="#607d8b", arrowsize=0.8];

  // 고수준 소비 모듈
  ui   [label="user_interface\nImGui 패널 · 이벤트 포워더", fillcolor="#e1bee7", color="#6a1b9a"];
  disp [label="display\n뷰포트 · Reshape",          fillcolor="#bbdefb", color="#1565c0"];
  rend [label="renderer\nModel 소유 · Render",       fillcolor="#bbdefb", color="#1565c0"];

  // 코어 / 씬
  model [label="model\nITransformable 구현\n(+ model_imp.h)", fillcolor="#bbdefb", color="#1565c0"];
  cam   [label="camera\n투영 · 뷰 행렬",            fillcolor="#bbdefb", color="#1565c0"];

  // 리소스 / 조명
  res   [label="resource_management\n텍스처 로딩 (stb)", fillcolor="#fff9c4", color="#f9a825"];
  light [label="lighting", fillcolor="#b2dfdb", color="#00695c"];

  // 헤더 전용 인터페이스
  trans  [label="transformable.h\nITransformable · PODTransform", fillcolor="#c8e6c9", color="#2e7d32"];
  consts [label="constants.h", fillcolor="#c8e6c9", color="#2e7d32"];

  // 독립 모듈 (intra-src 의존 없음 — apps/main.cpp 가 직접 사용)
  subgraph cluster_standalone {
    label="독립 모듈 (intra-src 의존 없음)"; labeljust="l"; fontname="Helvetica"; fontsize=9;
    style="rounded,filled"; fillcolor="#f5f5f5"; color="#cfd8dc";
    ground [label="ground",  fillcolor="#b2dfdb", color="#00695c"];
    sky    [label="skybox",  fillcolor="#b2dfdb", color="#00695c"];
    inp    [label="inputs\nKeybaordInput · MouseInput", fillcolor="#ffe0b2", color="#e65100"];
  }

  // 의존 관계 (#include)
  disp  -> cam;
  disp  -> model;
  rend  -> cam;
  rend  -> model;
  model -> trans;
  model -> res;
  ui    -> light;
  ui    -> trans;
  ui    -> consts;
}
\enddot

`model → resource_management` 엣지는 `model_imp.h` 가 `resource_management.h` 를
include 하기 때문이며, `display`·`renderer` 는 공통적으로 `camera`·`model` 에
의존합니다. `ground`·`skybox`·`inputs` 는 다른 `src` 모듈에 의존하지 않고
`apps/main.cpp` 가 직접 사용하는 독립 모듈입니다.

## 빌드 명령

```bash
# macOS / Linux: apps (기본)
cmake --preset ninja && cmake --build --preset ninja

# tutorials / samples
cmake --preset ninja-tutorials && cmake --build --preset ninja-tutorials
cmake --preset ninja-samples   && cmake --build --preset ninja-samples

# Release (apps)
cmake --preset ninja-release && cmake --build --preset ninja-release

# Windows (MSVC)
cmake --preset msvc && cmake --build --preset msvc

# 본 문서 빌드 (Doxygen 필요)
cmake --preset ninja && cmake --build build_ninja --target doxygen
```

## 프리셋 매트릭스

| Preset            | OS            | Generator           | binaryDir   |
|-------------------|---------------|---------------------|-------------|
| `ninja`           | macOS / Linux | Ninja (Debug)       | `build_ninja` |
| `ninja-release`   | macOS / Linux | Ninja (Release)     | `build_ninja` |
| `ninja-tutorials` | macOS / Linux | Ninja + tutorials   | `build_ninja-tutorials` |
| `ninja-samples`   | macOS / Linux | Ninja + samples     | `build_ninja-samples` |
| `msvc`            | Windows       | VS 17 2022 (x64)    | `build_msvc` |

## 빌드 옵션

| 옵션 | 기본값 | 설명 |
|------|--------|------|
| `SJH_OPENGL_BUILD_DOCS` | `ON`  | `doxygen` 타겟 등록 (`cmake/Doxygen.cmake`) |
| `BUILD_TUTORIALS`       | `OFF` | `apps` 대신 `tutorials/` 빌드 |
| `BUILD_SAMPLES`         | `OFF` | `apps` 대신 `samples/` 빌드 |

## 문서 자동 배포 (CI)

`.github/workflows/deploy-doxygen.yml` 가 다음 시점에 Doxygen HTML 을 생성해
GitHub Pages 로 배포합니다.

- `main` 브랜치 push (자동)
- `docs-deploy-*` 태그 push (명시적 수동 배포)
- `workflow_dispatch` (Actions UI 의 Run 버튼)

CI 는 `doc/Doxyfile.in` 의 `@`-변수를 `sed` 로 치환하므로,
로컬 `configure_file` 결과와 동일한 출력을 만듭니다.
