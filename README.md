
# 컴퓨터그래픽스및AI응용

<div align=center>
    <a href="https://www.youtube.com/watch?v=BvLO3C3dpKo&list=PLIMQzWccmG7YWLpXeiQRaBV94vfIJc2Ex&index=19"><img src="https://img.youtube.com/vi/BvLO3C3dpKo/hqdefault.jpg" width="480" alt="Video 1"></a>
    <h5>케로로 모델 (손 제외) 팔, 다리, 얼굴, 몸통 레퍼런스</h5>
    <a href="https://youtu.be/eOuZSZBm4mE?si=nD9nyNxMSjKj4QLK"><img src="https://img.youtube.com/vi/eOuZSZBm4mE/hqdefault.jpg" width="480" alt="Video"></a>
    <h5>케로로 모델 손(벙어리 손) 레퍼런스 -> 근데 그냥 도라에몽처럼 동그란 손으로 대체하는게 빠를지도.. </h5>
</div>

#### 추가 레퍼런스 이미지 
[📁 docs/metahuman](https://github.com/DogGuyMan/Computer-OpenGL-ComputerGraphics/tree/main/docs/metahuman)

#### 우선순위

1. [ ] 케로로 모델 (얼굴, 몸통, 팔 다리) -> 여기만 해도 과제 제출 용 최소 구현
2. [ ] Skybox 배경, 바닥
3. [ ] Ramp Cartoon Rendering & Back Face Culling + Inverted Hull Outline
4. [ ] 벙어리 손 구체화?

### 결과

<div align=center>
    <img src="image/스크린샷 2026-05-13 22.02.59.png" width="80%">
    <h5>머리</h5>
    <img src="image/스크린샷 2026-05-13 22.03.04.png" width="80%">
    <h5>몸통</h5>
    <img src="image/스크린샷 2026-05-13 22.03.08.png" width="80%">
    <h5>모자 (쌍곡면)</h5>
</div>

---

> ### 📄 사용한 툴


#### 1). 플랫폼

* Window (wine으로 window API macOS에서 실행)
* MacOs

---

#### 2). Build System 

* Makefile (Ninja)
* Visual Studio (MSVC)

---

#### 3). Dependency

##### Fetch_Content 사용
1. freeglut (MinGW)
1. OpenGL
2. spdlog
3. stb

---

> ### 📄 CMake


#### 1). Build System Generate

```
cmake --preset debug && cmake --build --preset debug
```

---

#### 2). Build Run

---
