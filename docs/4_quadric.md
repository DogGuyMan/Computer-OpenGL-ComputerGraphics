#### `void gluQuadricDrawStyle(GLUquadric* quad, GLenum draw);`
##### GLU(OpenGL Utility Library)의 quadric 객체(2차곡면) 렌더링 스타일을 설정하는 API입니다. 

##### `GLUquadric* quad`
* `gluNewQuadric()`는 quadric 객체 포인터 

##### `GLenum draw`

|값|효과|
|---|---|
|GLU_FILL (기본)|채워진 polygon으로 그림. 일반 surface 렌더링.|
|GLU_LINE|wireframe — 모든 edge를 line으로 그림.|
|GLU_SILHOUETTE|외곽선만 — 인접 polygon이 공유하지 않는 edge만. shape의 |윤곽 추출용.
|GLU_POINT|vertex 위치에 점만 찍음.|

#### 사용 패턴

```cpp
GLUquadric* q = gluNewQuadric();             // 1. 생성
gluQuadricDrawStyle(q, GLU_FILL);            // 2. 스타일
gluQuadricNormals(q, GLU_SMOOTH);            // 3. 노멀 (조명용)
gluQuadricTexture(q, GL_TRUE);               // 4. 텍스처 좌표 생성
gluSphere(q, /*radius*/1.0, /*slices*/32, /*stacks*/16);  // 5. 그리기
gluDeleteQuadric(q);                         // 6. 해제 (메모리 누수 방지)
```

#### `glutWireSphere(r, slices, stacks)` VS `gluSphere(q, r, slices, stacks)`

|소속|GLUT (윈도잉/유틸)|GLU (수학·primitive 유틸)|
|---|---|---|
|호출 형태|함수 하나|상태 객체 + 함수|
|draw style|함수 이름에 박힘 (Wire/Solid)|q의 멤버로 4종 (FILL/LINE/SILHOUETTE/POINT)|
|normal 옵션|없음 (조명 부정확하거나 고정)|q의 멤버 — NONE/FLAT/SMOOTH|
|texture 좌표|없음|q의 멤버 — ON/OFF|
|orientation|고정|q의 멤버 — OUTSIDE/INSIDE|
|그릴 수 있는 모양|sphere/cube/cone/teapot 등 각각 함수|같은 q로 sphere/cylinder/disk/partialDisk|
|객체 수명|없음|gluNewQuadric ↔ gluDeleteQuadric|

#### 설계 철학은

* 바로 Build Pattern을 착안해 함수 레퍼런스 인자로 전달하려고
* "drawStyle x normals x texture x orientation"의 조합 폭발을 함수 시그니처에 다 박지 않으려고.