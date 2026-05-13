# GIMP → OpenGL 텍스처 핫리로드 파이프라인 구축 가이드

GIMP에서 PSD/이미지를 편집·저장할 때 freeglut + 레거시 OpenGL 렌더러가 실시간으로 텍스처를 갱신하는 파이프라인은 **GIMP Python-Fu 플러그인(PNG 자동 내보내기) → efsw 파일 와처 → std::atomic 더티 플래그 → glTexSubImage2D** 조합으로 구현할 수 있다. 핵심 제약은 GIMP가 **저장/내보내기 이벤트 후킹을 제공하지 않는다**는 점이며, 이를 키보드 단축키에 바인딩된 커스텀 플러그인으로 우회해야 한다. 디즈니 Deep Canvas가 브러시스트로크를 3D 공간에 파라메트릭하게 기록한 것과 달리, 이 파이프라인은 파일 I/O 기반의 텍스처 갱신에 집중하므로 아키텍처적 단순함을 최대한 활용하는 것이 핵심이다.

---

## GIMP에는 저장 이벤트 후킹이 없다

GIMP의 플러그인 아키텍처에서 가장 중요한 사실: **Script-Fu든 Python-Fu든 저장/내보내기 이벤트를 후킹할 수 없다.** GIMP 내부 C 코드에는 `GimpImage` 객체에 `"saved"`, `"exported"` 시그널이 존재하지만, 이것은 PDB(Procedure Database)나 플러그인 API를 통해 외부에 노출되지 않는다. `file-overwrite`, `file-png-save` 등의 프로시저는 호출 가능한 함수일 뿐, 콜백을 등록할 수 있는 이벤트가 아니다.

**실용적 해결책은 세 가지다:**

**방법 A — 키보드 바인딩 커스텀 플러그인 (권장).** `Ctrl+S`를 대체하는 플러그인을 만들어, XCF 저장과 PNG 내보내기를 한 번에 수행한다. 아티스트의 워크플로를 거의 변경하지 않으면서 가장 확실하게 PNG를 생성한다. Python-Fu로 구현하면 `os.replace()`를 사용한 atomic rename이 가능하다.

**방법 B — 외부 파일 시스템 와처.** GIMP가 저장하는 XCF 또는 PSD 파일을 OS 레벨에서 감시하고, 변경 감지 시 외부 도구(`xcf2png`, ImageMagick)로 PNG를 생성한다. GIMP 내부 수정이 불필요하지만 변환 과정에 지연이 발생한다.

**방법 C — 방법 A + B 결합.** 플러그인이 PNG를 직접 내보내고, C++ 측 efsw 와처가 해당 PNG 파일을 감시한다. 가장 안정적이며 이 리포트의 권장 아키텍처다.

### GIMP 2.10 Python-Fu 자동 내보내기 플러그인 패턴

```python
from gimpfu import *
import os

def save_and_export(image, drawable, export_path):
    # XCF 저장
    pdb.gimp_xcf_save(0, image, drawable,
        image.filename, image.filename)
    # 복제 → 병합 → PNG 내보내기 (atomic)
    copy = pdb.gimp_image_duplicate(image)
    layer = pdb.gimp_image_merge_visible_layers(copy, CLIP_TO_IMAGE)
    tmp = export_path + ".tmp"
    pdb.file_png_save(copy, layer, tmp, "tmp.png", 0, 9, 1, 1, 1, 1, 1)
    pdb.gimp_image_delete(copy)
    os.replace(tmp, export_path)   # POSIX atomic rename
    pdb.gimp_image_clean_all(image)

register("python-fu-save-export", "Save + Export PNG", "",
    "Author", "MIT", "2026",
    "<Image>/File/Save and Export PNG...", "*",
    [(PF_STRING, "export_path", "Path", "/tmp/texture.png")],
    [], save_and_export)
main()
```

핵심은 **`os.replace()`로 임시 파일을 atomic rename**하는 부분이다. GIMP의 Export 플러그인들은 대상 파일에 직접 write하므로(temp+rename 미사용), 외부 와처가 불완전한 파일을 읽을 위험이 있다. 플러그인 측에서 atomic write를 보장하면 이 문제가 해결된다.

### GIMP 3.x에서의 주요 변경점

GIMP 3.0(2025년 3월 출시)은 플러그인 시스템을 대폭 개편했다. **Script-Fu는 `gimp-script-fu-interpreter-3.0` 기반으로 각 플러그인이 독립 프로세스에서 실행**되어, 하나의 버그가 전체 Script-Fu를 크래시시키지 않는다. Python-Fu는 Python 3 + GObject Introspection으로 전환되어 `gi.repository.Gimp` 모듈을 사용한다. `script-fu-register` → `script-fu-register-filter`로 변경, 파일 경로는 이중 문자열 대신 단일 GFile 문자열을 사용, v3 방언에서는 반환값이 리스트로 래핑되지 않는다. 새 스타일 플러그인은 GIMP 실행 중 핫에딧이 가능해 개발 사이클이 빠르다.

**Script-Fu vs Python-Fu 선택**: Script-Fu는 모든 플랫폼에서 기본 내장되어 가용성이 확실하지만, Python-Fu는 `os.replace()` 같은 파일시스템 조작과 복잡한 경로 처리에 강하다. macOS/Windows 크로스플랫폼에서는 **Script-Fu가 안전한 기본 선택**, 기능이 필요하면 Python-Fu를 사용한다.

---

## PSD 직접 파싱보다 PNG 중간 파일이 낫다

C++ 측에서 텍스처를 읽는 방식은 두 가지: PSD를 직접 파싱하거나, GIMP가 내보낸 PNG를 읽는 것이다. **레이어별 접근이 불필요하고 flattened RGBA 텍스처만 필요하다면, PNG + stb_image 조합이 압도적으로 유리하다.**

| 기준 | PSD 직접 파싱 (psd_sdk) | PNG 중간 파일 (stb_image) |
|------|------------------------|--------------------------|
| **구현 복잡도** | 중간 — 별도 라이브러리 필요 | **최소 — 단일 헤더** |
| **신뢰성** | PSD 포맷은 복잡하고 엣지케이스 다수 | **PNG는 완전히 표준화** |
| **파일 크기** | 50-500MB (복잡한 PSD) | **1-20MB (동일 해상도 PNG)** |
| **쓰기 완료 감지** | 대용량 → 감지 어려움 | 소용량 → 빠른 감지 |
| **레이어 접근** | ✅ 개별 레이어 읽기 가능 | ❌ flattened만 |

### C++ PSD 파싱 라이브러리 현황

**psd_sdk** (MolecularMatters)가 가장 성숙하다. GitHub **653스타**, BSD-2 라이선스, 8/16/32비트 지원, 개별 레이어 읽기 가능, STL/RTTI/예외 미사용. 2020년 오픈소스화된 상용 제품으로 코드 품질이 높다. 2단계 API(파싱 → 데이터 추출)로 병렬 처리가 가능하다. **PhotoshopAPI** (EmilDohne, **324스타**)는 C++20 기반으로 더 현대적이고 활발히 개발 중이며, psd_sdk 대비 **읽기 5-10배, 쓰기 20배 빠르다**고 주장하지만, flattened composite를 생성하지 않는다는 제약이 있다.

**stb_image도 PSD 읽기를 지원**하지만 composited view만 읽을 수 있고 레이어 접근은 불가능하다. 따라서 PSD 직접 파싱이 필요할 경우 psd_sdk, 그렇지 않으면 stb_image가 최선이다.

### HDR/EXR이 필요한 경우

HDR 텍스처가 필요하면 **tinyexr**(단일 헤더, BSD-3, HALF/FLOAT/UINT, 멀티파트 지원)을 권장한다. stb_image는 Radiance `.hdr`만 지원하고 EXR은 불가능하다. OpenEXR 공식 라이브러리는 VFX 파이프라인용으로 의존성이 무거워, 게임/도구 개발에는 tinyexr이 적합하다.

---

## OpenGL 핫리로드는 atomic 플래그와 glutIdleFunc로 충분하다

freeglut + 레거시 OpenGL(GL 2.x)에서 텍스처 핫리로드를 구현하는 표준 패턴은 놀랍도록 간단하다. OpenGL 컨텍스트는 단일 스레드에서만 호출해야 하므로, 파일 와처 스레드에서 `std::atomic<bool>`을 설정하고 메인 스레드에서 폴링하는 패턴이면 된다.

**`std::atomic<bool>`이 `std::mutex`보다 적합한 이유:** 단일 boolean 플래그만 공유하므로 뮤텍스의 커널 동기화 오버헤드가 불필요하다. `std::atomic<bool>`은 하드웨어 수준 원자 연산 한 번으로 컴파일되며, 기본 `memory_order_seq_cst`가 스레드 간 가시성을 보장한다. 파일 경로 등 추가 데이터를 함께 전달해야 할 때만 뮤텍스가 필요하다.

### 텍스처 업데이트: glTexImage2D vs glTexSubImage2D

텍스처 크기가 변하지 않으면 **`glTexSubImage2D`가 훨씬 효율적**이다. `glTexImage2D`는 매 호출마다 GPU 메모리를 재할당하지만, `glTexSubImage2D`는 기존 메모리에 픽셀 데이터만 DMA 전송한다. 텍스처 ID를 삭제/재생성할 필요 없이 같은 ID를 재바인딩하여 업데이트하면 된다.

```cpp
void reload_texture() {
    int w, h, ch;
    unsigned char* data = stbi_load("texture.png", &w, &h, &ch, 4);
    if (!data) { texture_dirty.store(true); return; } // 재시도

    glBindTexture(GL_TEXTURE_2D, g_texID);
    if (w != g_w || h != g_h) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, data);
        g_w = w; g_h = h;
    } else {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h,
                        GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    stbi_image_free(data);
}
```

### PBO(Pixel Buffer Object)는 핫리로드에 과잉

PBO는 **GL 2.1 코어 기능**으로 레거시 OpenGL에서 사용 가능하다. 더블 PBO 패턴은 GPU DMA 전송과 CPU 메모리 복사를 파이프라이닝하여 연속 스트리밍(비디오 30-60fps)에 탁월하다. 하지만 **핫리로드는 수 초~수 분 간격의 비연속 이벤트**이므로, 1024×1024 RGBA 텍스처 기준 직접 `glTexSubImage2D` 호출의 **1-5ms 지연은 무시 가능**하다. PBO의 코드 복잡도(버퍼 orphaning, 더블 버퍼링) 대비 이득이 거의 없다.

### freeglut 메인 루프 통합

**`glutIdleFunc`**은 이벤트 큐가 비어 있을 때 연속 호출되므로, atomic 플래그 폴링에 최적이다. 다만 CPU를 100% 사용하므로, 연속 애니메이션이 아니면 **`glutTimerFunc(100, callback, 0)`으로 100ms 간격 폴링**이 더 경제적이다. 텍스처 리로드 후 반드시 `glutPostRedisplay()`를 호출하여 재렌더링을 트리거한다.

---

## Deep Canvas는 텍스처 매핑이 아닌 3D 브러시스트로크 시스템이었다

1999년 SIGGRAPH에서 Eric Daniels가 발표한 디즈니 Deep Canvas의 핵심 혁신: **브러시스트로크를 완성된 이미지로 텍스처 매핑하는 것이 아니라, 각 스트로크를 3D 공간에 파라메트릭 이벤트로 기록**한 것이다. 아티스트가 2D 화면에서 와콤 태블릿으로 칠하면, 소프트웨어가 ray-intersection으로 3D 프록시 지오메트리 위의 위치를 계산하고, 스트로크의 위치·색상·브러시 타입·압력 등을 3D 좌표로 저장했다. 카메라가 이동하면 모든 스트로크가 새로운 시점에서 재렌더링되어 원근법이 자연스럽게 유지되었다.

Eric Daniels의 표현에 따르면 Deep Canvas는 **"피아노 자동연주 장치"**와 같아서, 화가의 페인팅 방식을 학습한 뒤 약간 다른 각도에서 수십~수백 개의 유사 페인팅을 자동 복제할 수 있었다. 타잔(1999)에서 약 10분, Treasure Planet(2002)에서는 **환경의 75%**에 적용되었고, 개발자들은 2003년 아카데미 기술상을 수상했다.

### 현대 유사 구현과 GIMP 파이프라인의 차이

**OverCoat** (ETH Zurich/Disney Research, SIGGRAPH 2011)은 Deep Canvas를 일반화하여 implicit canvas 위에 2D 스트로크를 3D 공간에 임베딩하는 연구다. **ArmorPaint**는 오픈소스 Substance Painter 대안으로, GPU 가속 3D PBR 텍스처 페인팅과 UE/Unity Live Link를 제공한다. **Blender-Krita Link 플러그인**은 Python 공유 메모리로 Krita와 Blender를 연결하여 파일 I/O 없이 텍스처를 실시간 동기화한다.

GIMP + OpenGL 파이프라인은 이들과 근본적으로 다르다. Deep Canvas나 Substance Painter가 **동일 프로세스 내에서 페인팅과 렌더링을 통합**하는 것과 달리, 이 파이프라인은 **별도 애플리케이션 간 파일 I/O 기반 연결**이다. Adobe의 R&D에 따르면, Substance Painter에서도 외부 렌더러 연결 시 디스크 I/O 대신 **named shared memory**(OS IPC)로 GL 텍스처를 직접 공유하는 방식을 연구했으며, 이것이 미래 발전 방향이 될 수 있다.

---

## 권장 최종 아키텍처와 CMake 통합

전체 파이프라인을 정리하면 다음과 같다:

```
GIMP Python-Fu 플러그인 (Ctrl+S 대체)
  ├─ gimp_image_duplicate → merge_visible_layers → file_png_save
  ├─ os.replace(tmp, target)  ← atomic write 보장
  └─ /textures/current.png 에 출력

efsw 파일 와처 스레드 (C++)
  ├─ /textures/current.png 감시
  ├─ Modified 이벤트 감지
  ├─ 500ms 디바운싱 (마지막 이벤트 후 안정 확인)
  └─ texture_dirty.store(true)

freeglut 메인 루프
  ├─ glutIdleFunc 또는 glutTimerFunc(100ms)
  ├─ if (texture_dirty.load()) → reload_texture()
  ├─ stbi_load("current.png") → glTexSubImage2D
  └─ glutPostRedisplay()
```

### macOS + Windows 크로스플랫폼 가동 가능성

이 파이프라인은 **양 플랫폼에서 완전히 작동**한다. efsw는 macOS에서 FSEvents, Windows에서 I/O Completion Ports를 사용하며, stb_image와 freeglut 모두 크로스플랫폼이다. GIMP Script-Fu는 모든 플랫폼에서 기본 내장되어 있고, Python-Fu는 GIMP 3.x에서 Python 3 + PyGObject가 제공되면 사용 가능하다. `os.replace()`는 POSIX에서 atomic이고 Windows에서도 동일 볼륨 내에서 atomic이다.

### CMake로 efsw 통합하는 방법

```cmake
cmake_minimum_required(VERSION 3.14)
project(TextureHotReload)

include(FetchContent)

# efsw 파일 와처
FetchContent_Declare(
    efsw
    GIT_REPOSITORY https://github.com/SpartanJ/efsw.git
    GIT_TAG master
)
FetchContent_MakeAvailable(efsw)

# stb_image (헤더만 필요)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(renderer main.cpp)
target_link_libraries(renderer PRIVATE
    efsw
    OpenGL::GL
    GLUT::GLUT
)
target_include_directories(renderer PRIVATE ${stb_SOURCE_DIR})
```

efsw는 자체 `CMakeLists.txt`와 `efswConfig.cmake.in`을 포함하므로 FetchContent로 깔끔하게 통합된다. vcpkg(`vcpkg install efsw`)나 Conan으로도 설치 가능하다. 대안으로 **dmon**(단일 헤더 C99, BSD-2, ~550 스타)은 `#define DMON_IMPL` + `#include "dmon.h"`만으로 통합되어 빌드 시스템 설정이 최소화된다.

### 디바운싱 구현 핵심

파일 와처는 단일 저장에 대해 **3-5개의 이벤트**를 발생시킬 수 있다. 플러그인 측 atomic rename을 사용하더라도 안전을 위해 C++ 측에서 500ms 디바운싱을 구현해야 한다:

```cpp
// efsw 콜백에서
void handleFileAction(efsw::WatchID, const std::string&,
    const std::string& filename, efsw::Action action, std::string) {
    if (action == efsw::Actions::Modified && filename == "current.png") {
        last_event_time = std::chrono::steady_clock::now();
    }
}

// 메인 루프에서
auto elapsed = std::chrono::steady_clock::now() - last_event_time;
if (elapsed > 500ms && !texture_dirty.load()) {
    texture_dirty.store(true);
}
```

## Conclusion

이 파이프라인의 실용성은 **높다**. 모든 구성 요소가 성숙하고 크로스플랫폼이며, 의존성이 최소화되어 있다. GIMP 플러그인에서 atomic PNG 내보내기 → efsw 감시 → stb_image 로딩 → glTexSubImage2D 업데이트의 총 지연은 **저장 후 1초 이내**로 기대할 수 있다. 향후 지연을 더 줄이려면 Adobe R&D처럼 named shared memory IPC로 디스크 I/O를 완전히 제거하는 방향을 고려할 수 있으나, 파일 I/O 기반 방식만으로도 아티스트-렌더러 피드백 루프는 충분히 실용적이다.