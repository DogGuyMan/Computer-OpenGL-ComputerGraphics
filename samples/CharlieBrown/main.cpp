#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define M_PIf 3.14159265f

float rotateX = -10.0f, rotateY = 0.0f;
float zoom = 5.0f;
int lastX, lastY;
bool isDragging = false;
GLuint textureID_zigzag; // 몸통 텍스처
GLuint textureID_shoes; // 신발 텍스처
GLuint textureID_grass; // 바닥 텍스처
GLuint textureID_background; // 배경 텍스처


void drawCharlieHead() {

    // 머리
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glColor3f(1.0f, 0.8f, 0.6f);
    glutSolidSphere(0.5f, 40, 40);
    glPopMatrix();

    // ===== 코 =====
    glPushMatrix();
    //glColor3f(1.0f, 0.75f, 0.6f);
    glColor3f(1.0f, 0.8f, 0.6f); // 살색 동일
    glTranslatef(0.0f, -0.02f, 0.50f); // 코 위치 (입 위, 머리보다 살짝 앞으로)
    glutSolidSphere(0.075f, 20, 20); // 작은 구체
    glPopMatrix();

    // ===== 입 선 =====
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(3.0f);
    
    float leftX = -0.24f;
    float rightX = 0.24f;
    float r = 0.5f;

    glBegin(GL_LINE_STRIP);
    for (float x = leftX; x <= rightX; x += 0.01f) {
        float y = -0.3f + 2.0f * x * x; // 아래로 벌어진 포물선
        float z = sqrt(r * r - x * x - y * y) + 0.005f; // 구 표면 위 z좌표
        glVertex3f(x, y, z);
    }
    glEnd();

    glLineWidth(1.0f); // 복구

    // ===== 입 양 끝 점 =====
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glColor3f(0.0f, 0.0f, 0.0f);

    // 왼쪽 입꼬리
    float lx = leftX;
    float ly = -0.3f + 2.0f * lx * lx;
    float lz = sqrt(r * r - lx * lx - ly * ly) + 0.005f;
    glVertex3f(lx, ly, lz);

    // 오른쪽 입꼬리
    float rx = rightX;
    float ry = -0.3f + 2.0f * rx * rx;
    float rz = sqrt(r * r - rx * rx - ry * ry) + 0.005f;
    glVertex3f(rx, ry, rz);
    glEnd();

    glPointSize(1.0f); // 복구


    // ===== 눈 =====
    glColor3f(0.2f, 0.2f, 0.2f);
    glPointSize(6.0f);

    // 왼쪽 눈
    glPushMatrix();
    glTranslatef(-0.11f, 0.03f, 0.48f); // 왼쪽(좌), 위쪽(+y), 앞쪽으로 약간 돌출
    glutSolidSphere(0.015f, 16, 16); // 작고 동그란 눈
    glPopMatrix();

    // 오른쪽 눈
    glPushMatrix(); 
    glTranslatef(0.11f, 0.03f, 0.48f); // 오른쪽(우), 위쪽(+y), 앞쪽
    glutSolidSphere(0.015f, 16, 16);
    glPopMatrix();

    // ===== 눈썹 =====
    glColor3f(0.0f, 0.0f, 0.0f); // 검정색
    glLineWidth(4.0f);

    // 왼쪽 눈썹
    glBegin(GL_LINE_STRIP);
    for (float t = -0.025f; t <= 0.025f; t += 0.01f) {
        float x = -0.13f + t;
        float y = 0.18f - 8.0f * t * t; // 위로 약간 휘는 포물선
        float z = sqrt(r * r - x * x - y * y) + 0.005f;
        glVertex3f(x, y, z);
    }
    glEnd();

    // 오른쪽 눈썹
    glBegin(GL_LINE_STRIP);
    for (float t = -0.025f; t <= 0.025f; t += 0.01f) {
        float x = 0.13f + t;
        float y = 0.18f - 8.0f * t * t; // 위로 약간 휘는 포물선
        float z = sqrt(r * r - x * x - y * y) + 0.005f;
        glVertex3f(x, y, z);
    }
    glEnd();

    glLineWidth(1.0f); // 복구


    // ===== 귀 =====
    glColor3f(1.0f, 0.8f, 0.6f); // 머리와 동일한 색상

    // 왼쪽 귀
    glPushMatrix();
    glTranslatef(-0.53f, -0.06f, 0.0f);   // 머리 왼쪽 옆
    glRotatef(0.0f, 0, 1, 0);         // 정면을 향하도록 회전
    glScalef(1.0f, 1.1f, 0.5f);        // 타원형으로 눌림
    glutSolidSphere(0.1f, 20, 20);    // 약간 작은 귀
    glPopMatrix();

    // 오른쪽 귀
    glPushMatrix();
    glTranslatef(0.53f, -0.06f, 0.0f);    // 머리 오른쪽 옆
    glRotatef(0.0f, 0, 1, 0);        // 반대방향으로 회전
    glScalef(1.0f, 1.1f, 0.5f);        // 타원형으로 눌림
    glutSolidSphere(0.1f, 20, 20);
    glPopMatrix();


    // ===== 머리카락 (개선된 W자 Bézier 곡선) =====
    glColor3f(0.0f, 0.0f, 0.0f); // 검정색
    glLineWidth(6.0f);

    glPushMatrix();
    glTranslatef(0.0f, -0.355f, 0.0f);
    glScalef(1.2f, 1.2f, 1.0f);

    auto drawBezier = [](GLfloat p0[3], GLfloat p1[3], GLfloat p2[3], GLfloat p3[3]) {
        for (float t = 0.0f; t <= 1.0f; t += 0.05f) {
            float u = 1 - t;
            float x = u * u * u * p0[0] + 3 * u * u * t * p1[0] + 3 * u * t * t * p2[0] + t * t * t * p3[0];
            float y = u * u * u * p0[1] + 3 * u * u * t * p1[1] + 3 * u * t * t * p2[1] + t * t * t * p3[1];
            float z = u * u * u * p0[2] + 3 * u * u * t * p1[2] + 3 * u * t * t * p2[2] + t * t * t * p3[2];
            glVertex3f(x, y, z);
        }
    };

    // (1) 중앙 고리
    GLfloat loop0[3] = { 0.00f, 0.50f, 0.4584f };
    GLfloat loop1[3] = { 0.03f, 0.58f, 0.4501f };
    GLfloat loop2[3] = { -0.03f, 0.58f, 0.4501f };
    GLfloat loop3[3] = { 0.00f, 0.50f, 0.4584f };

    // (2) 왼쪽 곡선
    GLfloat left0[3] = { 0.00f, 0.50f, 0.4584f };
    GLfloat left1[3] = { -0.05f, 0.46f, 0.4571f };
    GLfloat left2[3] = { -0.17f, 0.46f, 0.4349f };
    GLfloat left3[3] = { -0.15f, 0.55f, 0.4344f };

    // (3) 오른쪽 곡선
    GLfloat right0[3] = { -0.0028f, 0.51f, 0.4583f };
    GLfloat right1[3] = { 0.05f, 0.46f, 0.4571f };
    GLfloat right2[3] = { 0.17f, 0.46f, 0.4349f };
    GLfloat right3[3] = { 0.147f, 0.547f, 0.4353f };

    // (4) 오른쪽 끝에서 왼쪽으로 휘는 곡선
    GLfloat reflect0[3] = { 0.15f, 0.53f, 0.4357f };
    GLfloat reflect1[3] = { 0.13f, 0.58f, 0.4273f };
    GLfloat reflect2[3] = { 0.09f, 0.60f, 0.4220f };
    GLfloat reflect3[3] = { 0.07f, 0.57f, 0.4211f };

    // ★ 각 Bézier 곡선을 독립적으로 그리기 ★
    glBegin(GL_LINE_STRIP);
    drawBezier(loop0, loop1, loop2, loop3);
    glEnd();

    glBegin(GL_LINE_STRIP);
    drawBezier(left0, left1, left2, left3);
    glEnd();

    glBegin(GL_LINE_STRIP);
    drawBezier(right0, right1, right2, right3);
    glEnd();

    glBegin(GL_LINE_STRIP);
    drawBezier(reflect0, reflect1, reflect2, reflect3);
    glEnd();

    glPopMatrix();

    glLineWidth(1.0f);


}

void drawCharlieBody() {

    // ===== 목 (원기둥) =====
    GLUquadric* neck = gluNewQuadric();

    glPushMatrix();
    glColor3f(1.0f, 0.8f, 0.6f); // 머리/피부색과 동일
    glTranslatef(0.0f, -0.57f, 0.0f); // 머리 아래에 위치
    glRotatef(-90.0f, 1, 0, 0); // y축 중심 원기둥을 z축 방향으로 회전
    gluCylinder(neck, 0.12f, 0.12f, 0.15f, 20, 20); // 아래에서 위로 높이 0.15
    glPopMatrix();
    
    // ===== 몸통 (넓어지는 원기둥 + 상하 원판) =====
    int stacks = 20;
    int slices = 40;
    float height = 0.6f;

    glPushMatrix();
    glTranslatef(0.0f, -1.15f, 0.0f);  // 목 아래 위치
    // 텍스처 활성화
    glColor3f(1.0f, 1.0f, 1.0f); // 텍스처 색 왜곡 방지
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID_zigzag);  // 미리 loadTexture에서 로드한 텍스처
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    for (int i = 0; i < stacks; ++i) {
        float y0 = (float)i / stacks * height;
        float y1 = (float)(i + 1) / stacks * height;

        // 곡선 뒤집기: 아래쪽이 좁고 위쪽이 넓게
        float t0 = (float)(i) / stacks;
        float t1 = (float)(i + 1) / stacks;

        float r0 = 0.12f + 0.17f * (1 - powf(t0, 1.5f));
        float r1 = 0.12f + 0.17f * (1 - powf(t1, 1.5f));

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * M_PIf * j / slices;
            float x0 = r0 * cos(theta);
            float z0 = r0 * sin(theta);
            float x1 = r1 * cos(theta);
            float z1 = r1 * sin(theta);

            float s = (float)j / slices;
            float baseShift = 0.1f;
            glNormal3f(cos(theta), 0.0f, sin(theta));

            glTexCoord2f(s, t0 + baseShift); 
            glVertex3f(x0, y0, z0);
            glTexCoord2f(s, t1 + baseShift); 
            glVertex3f(x1, y1, z1);

        }
        glEnd();

    }

    glDisable(GL_TEXTURE_2D);

    // 하단 원판 (bottom cap)
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.9686f, 0.7255f, 0.1804f); // 찰리 브라운 옷 주황색 rgb(247, 185, 46)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.5f, 0.5f); // 중심
    glVertex3f(0.0f, 0.0f, 0.0f);  

    for (int j = 0; j <= slices; ++j) {
        float theta = 2.0f * M_PIf * j / slices;
        float r = 0.29f;
        float x = r * cos(theta);
        float z = r * sin(theta);

        // 텍스처 좌표를 (0~1) 범위 안에서 원형 매핑
        float s = 0.5f + 0.5f * cos(theta);
        float t = 0.5f + 0.5f * sin(theta);
        glTexCoord2f(s, t);
        glVertex3f(x, 0.0f, z);

    }
    glEnd();


    glPopMatrix();

    // 몸통 카라 (∧형 컷팅, 위쪽 평평하게 유지)
    int c_slices = 60;
    float outerTop = 0.13f, outerBottom = 0.17f;
    float innerTop = 0.10f, innerBottom = 0.15f;
    float c_height = 0.08f;

    float cutCenter = M_PIf / 2.0f;
    float cutRange = M_PIf / 9.0f;
    float peakHeight = 0.07f;  // 얼마나 파낼지 조절

    glPushMatrix();
    glTranslatef(0.0f, -0.55f, 0.0f);
    glColor3f(0.9686f, 0.7255f, 0.1804f);

    auto getYOffset = [&](float angle) {
        float diff = fabs(angle - cutCenter);
        if (diff < cutRange)
            return peakHeight * (1.0f - diff / cutRange); // 정면이 가장 아래로 내려감
        return 0.0f;
        };

    // 1. 바깥면
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= c_slices; ++i) {
        float angle = 2.0f * M_PIf * i / c_slices;
        float yOffset = getYOffset(angle);

        float x1 = outerTop * cos(angle);
        float z1 = outerTop * sin(angle);
        float x2 = outerBottom * cos(angle);
        float z2 = outerBottom * sin(angle);

        glVertex3f(x1, 0.0f, z1);                 // 평평한 윗부분 유지
        glVertex3f(x2, -c_height + yOffset, z2);  // 아랫부분만 패임 적용
    }
    glEnd();

    // 2. 안쪽면
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= c_slices; ++i) {
        float angle = 2.0f * M_PIf * i / c_slices;
        float yOffset = getYOffset(angle);

        float x1 = innerBottom * cos(angle);
        float z1 = innerBottom * sin(angle);
        float x2 = innerTop * cos(angle);
        float z2 = innerTop * sin(angle);

        glVertex3f(x1, -c_height + yOffset, z1);  // 아랫부분만 패임
        glVertex3f(x2, 0.0f, z2);                 // 윗부분 평평하게 유지
    }
    glEnd();

    // 3. 상단면 (flat top)
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= c_slices; ++i) {
        float angle = 2.0f * M_PIf * i / c_slices;

        float x1 = outerTop * cos(angle);
        float z1 = outerTop * sin(angle);
        float x2 = innerTop * cos(angle);
        float z2 = innerTop * sin(angle);

        glVertex3f(x1, 0.0f, z1); // yOffset 없음
        glVertex3f(x2, 0.0f, z2);
    }
    glEnd();

    // 4. 하단면
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= c_slices; ++i) {
        float angle = 2.0f * M_PIf * i / c_slices;
        float yOffset = getYOffset(angle);

        float x1 = outerBottom * cos(angle);
        float z1 = outerBottom * sin(angle);
        float x2 = innerBottom * cos(angle);
        float z2 = innerBottom * sin(angle);

        glVertex3f(x1, -c_height + yOffset, z1);
        glVertex3f(x2, -c_height + yOffset, z2);
    }
    glEnd();

    glPopMatrix();


}

void drawPants() {
    GLUquadric* quad = gluNewQuadric();
    gluQuadricDrawStyle(quad, GLU_FILL); // <- 꼭 추가!

    // 왼쪽 다리
    glPushMatrix();
    glTranslatef(-0.13f, -1.1f, 0.0f); // 몸 아래로 이동
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(1.0f, 1.3f, 0.8f);         // 타원형 기둥처럼 스케일 조정
    glColor3f(0.0157f, 0.3922f, 0.6784f);
    gluCylinder(quad, 0.15f, 0.15f, 0.2f, 20, 20);
    glPopMatrix();

    // 내부 원통
    glPushMatrix();
    glTranslatef(-0.13f, -1.1f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(0.97f, 1.3f, 0.78f); // 살짝 더 작은 반지름
    glColor3f(0.0118f, 0.2941f, 0.5490f); // 안쪽 어두운 파란색
    gluCylinder(quad, 0.149f, 0.149f, 0.2f, 20, 20);
    glPopMatrix();

    // 오른쪽 다리
    glPushMatrix();
    glTranslatef(0.13f, -1.1f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(1.0f, 1.3f, 0.8f);
    glColor3f(0.0157f, 0.3922f, 0.6784f);
    gluCylinder(quad, 0.15f, 0.15f, 0.2f, 20, 20);
    glPopMatrix();

    // 내부 원통
    glPushMatrix();
    glTranslatef(0.13f, -1.1f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(0.97f, 1.3f, 0.78f); // 살짝 더 작은 반지름
    glColor3f(0.0118f, 0.2941f, 0.5490f); // 안쪽 어두운 파란색
    gluCylinder(quad, 0.149f, 0.149f, 0.2f, 20, 20);
    glPopMatrix();

    // 왼쪽 다리 윗면 덮개
    glPushMatrix();
    glTranslatef(-0.13f, -1.16f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(1.0f, 1.3f, 0.8f);
    gluDisk(quad, 0.0f, 0.15f, 20, 1);  // 바깥 반지름과 동일
    glPopMatrix();

    // 오른쪽 다리 윗면 덮개
    glPushMatrix();
    glTranslatef(0.13f, -1.16f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(1.0f, 1.3f, 0.8f);
    gluDisk(quad, 0.0f, 0.15f, 20, 1);
    glPopMatrix();
}

void drawLegs() {
    GLUquadric* quad = gluNewQuadric();

    glColor3f(0.9f, 0.7f, 0.5f);

    // 왼쪽 다리
    glPushMatrix();
    glTranslatef(-0.13f, -1.2f, 0.0f); // 바지보다 아래
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // x축 기준 회전
    glScalef(1.0f, 1.1f, 0.8f); // 살짝 타원형
    gluCylinder(quad, 0.06f, 0.06f, 0.2f, 20, 20);
    glPopMatrix();

    // 오른쪽 다리
    glPushMatrix();
    glTranslatef(0.13f, -1.2f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(1.0f, 1.1f, 0.8f);
    gluCylinder(quad, 0.06f, 0.06f, 0.2f, 20, 20);
    glPopMatrix();

    // 왼쪽 양말
    glPushMatrix();
    glTranslatef(-0.13f, -1.34f, 0.0f); // 다리보다 살짝 아래
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(1.0f, 1.1f, 1.3f);
    glColor3f(0.87f, 0.65f, 0.16f);
    gluCylinder(quad, 0.065f, 0.065f, 0.05f, 20, 20);
    glPopMatrix();

    // 오른쪽 양말
    glPushMatrix();
    glTranslatef(0.13f, -1.34f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(1.0f, 1.1f, 1.3f);
    glColor3f(0.87f, 0.65f, 0.16f);
    gluCylinder(quad, 0.065f, 0.065f, 0.05f, 20, 20);
    glPopMatrix();

}

void drawShoes() {
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE); // 텍스처 매핑 활성화

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID_shoes); // 이미 로드된 텍스처 사용
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glColor3f(1.0f, 1.0f, 1.0f);

    // 텍스처 좌표계를 180도 회전
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.5f, 0.5f, 0.0f);        // 중심으로 이동
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);   // 180도 회전
    glTranslatef(-0.5f, -0.4f, 0.0f);      // 원래 위치로 복귀
    glMatrixMode(GL_MODELVIEW);           // 모델뷰 복귀

    double plane[] = { 0.0, -1.0, 0.0, 0.06 };

    // ===== 왼쪽 신발 =====
    GLUquadric* leftShoe = gluNewQuadric();
    gluQuadricTexture(leftShoe, GL_TRUE); // 텍스처 좌표 생성 활성화

    glPushMatrix();
    glTranslatef(-0.13f, -1.46f, 0.03f); // 다리 아래로 이동
    glScalef(1.1f, 0.9f, 2.0f);          // 타원형 신발
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    glClipPlane(GL_CLIP_PLANE0, plane);
    glEnable(GL_CLIP_PLANE0);
    gluSphere(leftShoe, 0.1f, 30, 30);   // 텍스처 매핑 가능
    glDisable(GL_CLIP_PLANE0);
    glPopMatrix();

    gluDeleteQuadric(leftShoe);

    // ===== 오른쪽 신발 =====
    GLUquadric* rightShoe = gluNewQuadric();
    gluQuadricTexture(rightShoe, GL_TRUE);

    glPushMatrix();
    glTranslatef(0.13f, -1.46f, 0.03f);
    glScalef(1.1f, 0.9f, 2.0f);
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    glClipPlane(GL_CLIP_PLANE0, plane);
    glEnable(GL_CLIP_PLANE0);
    gluSphere(rightShoe, 0.1f, 30, 30);
    glDisable(GL_CLIP_PLANE0);
    glPopMatrix();

    gluDeleteQuadric(rightShoe);

    // 텍스처 행렬 복구
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_TEXTURE_2D);

    glColor3f(0.5725f, 0.2706f, 0.1412f);

    // 바닥 디스크 (왼쪽)
    glPushMatrix();
    glTranslatef(-0.13f, -1.51f, 0.03f);
    glRotatef(-90.0f, 1, 0, 0);
    glScalef(0.9f, 1.6f, 1.0f);
    gluDisk(quad, 0.0f, 0.1f, 30, 1);
    glPopMatrix();

    // 바닥 디스크 (오른쪽)
    glPushMatrix();
    glTranslatef(0.13f, -1.51f, 0.03f);
    glRotatef(-90.0f, 1, 0, 0);
    glScalef(0.9f, 1.6f, 1.0f);
    gluDisk(quad, 0.0f, 0.1f, 30, 1);
    glPopMatrix();
}

void drawCharlieArms() {

    // 왼쪽 소매 (팔꿈치에서 넓어지게)
    GLUquadric* quad = gluNewQuadric();

    glPushMatrix();
    glTranslatef(-0.22f, -0.67f, 0.0f); // 팔꿈치 위치에 맞게 배치
    glRotatef(65.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(90.0f, 0.0f, -1.0f, 0.0f); // 팔꿈치와 같은 회전 적용
    glColor3f(0.8510f, 0.6549f, 0.1647f);
    glScalef(0.5f, 0.4f, 0.4f); // 소매 넓이 조정 (팔꿈치에서 몸통 비율에 맞게)
    gluCylinder(quad, 0.13f, 0.2f, 0.4f, 20, 20); // 소매는 팔꿈치에서 점점 넓어짐
    glPopMatrix();

    //왼쪽 어깨
    glPushMatrix();
    glTranslatef(-0.19f, -0.68f, 0.0f);
    glScalef(0.9f, 0.7f, 0.7f);
    gluSphere(quad, 0.1f, 30, 30);
    glPopMatrix();


    // 오른쪽 소매
    glPushMatrix();
    glTranslatef(0.22f, -0.67f, 0.0f); // 팔꿈치 위치에 맞게 배치
    glRotatef(65.0f, 0.0f, 0.0f, -1.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // 팔꿈치와 같은 회전 적용
    glColor3f(0.8510f, 0.6549f, 0.1647f);
    glScalef(0.5f, 0.4f, 0.4f); // 소매 넓이 조정 (팔꿈치에서 몸통 비율에 맞게)
    gluCylinder(quad, 0.13f, 0.2f, 0.4f, 20, 20); // 소매는 팔꿈치에서 점점 넓어짐
    glPopMatrix();

    // 오른쪽 어깨
    glPushMatrix();
    glTranslatef(0.19f, -0.68f, 0.0f);
    glScalef(0.9f, 0.7f, 0.7f);
    gluSphere(quad, 0.1f, 30, 30);
    glPopMatrix();

    // === 왼쪽 살색 팔 (원기둥 추가) ===
    glPushMatrix();
    glColor3f(1.0f, 0.8f, 0.6f); // 살색 (팔 색상)
    glTranslatef(-0.32f, -1.0f, 0.0f); // 팔이 시작하는 대략적인 위치 (조정 필요)
    glRotatef(75.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glScalef(0.4f, 0.3f, 0.3f);
    gluCylinder(quad, 0.18f, 0.18f, 0.8f, 20, 20); // 반지름 0.06, 길이 0.25 (조정 필요)
    glPopMatrix();

    // === 오른팔 원기둥 ===
    glPushMatrix();
    glTranslatef(0.32f, -1.0f, 0.0f); // 팔이 시작하는 대략적인 위치 (조정 필요)
    glRotatef(75.0f, 0.0f, 0.0f, -1.0f);
    glRotatef(90.0f, 0.0f, -1.0f, 0.0f);
    glScalef(0.4f, 0.3f, 0.3f);
    gluCylinder(quad, 0.18f, 0.18f, 0.8f, 20, 20); // 반지름 0.06, 길이 0.25 (조정 필요)
    glPopMatrix();

    // 왼쪽 주먹
    glPushMatrix();
    glTranslatef(-0.32f, -1.0f, 0.0f);
    glScalef(0.7f, 0.9f, 0.9f);
    gluSphere(quad, 0.1f, 30, 30);
    glPopMatrix();

    // 오른쪽 주먹
    glPushMatrix();
    glTranslatef(0.32f, -1.0f, 0.0f);
    glScalef(0.7f, 0.9f, 0.9f);
    gluSphere(quad, 0.1f, 30, 30);
    glPopMatrix();

}

void drawGrassGround() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID_grass);

    glColor3f(1.0f, 1.0f, 1.0f); // 텍스처 색상 왜곡 방지

    float halfSize = 15.0f;    // 원래 5 -> 15으로 3배 확장
    float repeat = 15.0f;

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, -1.55f, -halfSize);
    glTexCoord2f(repeat, 0.0f); glVertex3f(halfSize, -1.55f, -halfSize);
    glTexCoord2f(repeat, repeat); glVertex3f(halfSize, -1.55f, halfSize);
    glTexCoord2f(0.0f, repeat); glVertex3f(-halfSize, -1.55f, halfSize);
    glEnd();


    glDisable(GL_TEXTURE_2D);
}

void drawSkyWall() {
    if (textureID_background == 0) {
        printf("??");
        return;
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID_background);


    glColor3f(1.0f, 1.0f, 1.0f); // 텍스처 색상 왜곡 방지

    float grassY = -1.8f;
    float height = 10.0f;
    float width = height * (933.0f / 768.0f);  // = 약 12.14f
    float depth = -15.0f;

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-width, grassY, depth);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width, grassY, depth);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(width, grassY + height, depth);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-width, grassY + height, depth);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}





void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(
        0.0, 0.0, zoom,
        0.0, 0.0, 0.0,
        0.0, 1.0, 0.0
    );

    GLfloat lightPos[] = { 0.0f, 2.0f, 5.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // 전체 캐릭터를 위로 살짝 이동시켜 머리를 위쪽에 배치
    glTranslatef(0.0f, 0.7f, 0.0f);

    glRotatef(rotateX, 1, 0, 0);
    glRotatef(rotateY, 0, 1, 0);

    drawSkyWall();
    drawGrassGround();

    drawCharlieHead();
    drawCharlieBody();
    drawPants();
    drawLegs();
    drawShoes();
    drawCharlieArms();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int, int) {
    if (key == 'a') zoom *= 0.9f;
    if (key == 'z') zoom *= 1.1f;
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isDragging = true;
        lastX = x;
        lastY = y;
    } else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        isDragging = false;
    }
#ifdef __APPLE__
    // macOS GLUT: 마우스 휠이 button 3(up)/4(down)으로 전달
    if (state == GLUT_DOWN) {
        if (button == 3) { zoom -= 0.4f; glutPostRedisplay(); }
        if (button == 4) { zoom += 0.4f; glutPostRedisplay(); }
        if (zoom < 1.0f) zoom = 1.0f;
        if (zoom > 5.0f) zoom = 5.0f;
    }
#endif
}

void motion(int x, int y) {
    if (isDragging) {
        rotateX += (y - lastY);
        rotateY += (x - lastX);
        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}

void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat lightPos[] = { 1.0f, 1.0f, 2.0f, 1.0f };  // 조명 위치
    GLfloat ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };  // 환경광
    GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };  // 난반사광
    GLfloat specular[] = { 0.4f, 0.4f, 0.4f, 1.0f };  // 반사광

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    glEnable(GL_COLOR_MATERIAL); // 색상으로 재질 표현
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void mouseWheel(int wheel, int direction, int x, int y) {
    if (direction > 0) zoom -= 0.4f;
    else zoom += 0.4f;

    if (zoom < 1.0f) zoom = 1.0f;
    if (zoom > 5.0f) zoom = 5.0f;

    glutPostRedisplay();
}

GLuint loadTexture(const char* filename) {
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (!image) {
        printf("Failed to load texture: %s\n", filename);
        return 0;
    }

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image);
    return texID;
}

void idle() {
    // 강제 렌더링을 수행하기 위한 함수
    glutPostRedisplay();
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Charlie Brown");

    // 기본 설정
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    // 조명 설정
    setupLighting();

    // 텍스처 로딩
    glEnable(GL_TEXTURE_2D);
    textureID_zigzag = loadTexture("zigzag_texture.png");
    textureID_shoes = loadTexture("shoe_texture.png");
    textureID_grass = loadTexture("grassground.png");
    textureID_background = loadTexture("background.png"); 

    // 배경색 설정
    glClearColor(0.494f, 0.655f, 0.898f, 1.0f);

    // 콜백 등록
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
#ifdef FREEGLUT
    glutMouseWheelFunc(mouseWheel);
#endif
    glutIdleFunc(idle); 

    // 메인 루프 시작 (이 아래는 절대 실행 안 됨)
    glutMainLoop();

    return 0;
}