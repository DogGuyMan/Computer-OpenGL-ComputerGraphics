#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

#include <stdbool.h>  

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

GLuint plushTexture;
GLuint feltTexture;

void loadTexture(const char* filename, GLuint* textureID);

float angleX = 0.0f, angleY = 0.0f;
int lastX, lastY;
bool isDragging = false;
float zoomZ = 4.0f;  // ī�޶� z�� �Ÿ� (�⺻��)

// �� ���
void drawRedFlower() {
    glPushMatrix();

    // ȸ��
    glRotatef(-40, 1, 0, 0);
    glRotatef(15, 0, 1, 0);
    glRotatef(50, 0, 0, 1);

    // ��ġ �̵�
    glTranslatef(0.3f, -0.1f, 1.0f);

    for (int i = 0; i < 5; i++) {
        float angle = i * 72.0f;
        float rad = angle * 3.14159f / 180.0f;
        float dx = 0.18f * cos(rad);
        float dy = 0.18f * sin(rad);

        glPushMatrix();
        glTranslatef(dx, dy, 0.0f);
        glRotatef(angle, 0, 0, 1);
        glScalef(1.4f, 1.0f, 1.0f);
        glColor3f(0.9f, 0.0f, 0.0f);  // ������
        glutSolidSphere(0.13, 20, 20);
        glPopMatrix();
    }

    glColor3f(1.0f, 1.0f, 0.2f);  // ��� �߽�
    glutSolidSphere(0.08, 20, 20);
    glPopMatrix();
}

void drawWhiteFlower(float tx, float ty, float tz, int rxangle, int ryangle, int rzangle) {
    glPushMatrix();

    // 1. ��ġ ����
    glTranslatef(tx, ty, tz);

    // 2. ȸ�� ���� 
    glRotatef(rxangle, 1, 0, 0);
    glRotatef(ryangle, 0, 1, 0);
    glRotatef(rzangle, 0, 0, 1);

    // 3. ���� 5�� (Ÿ����)
    for (int i = 0; i < 5; i++) {
        float angle = i * 72.0f;
        float rad = angle * 3.14159f / 180.0f;
        float dx = 0.18f * cos(rad);
        float dy = 0.18f * sin(rad);

        glPushMatrix();
        glTranslatef(dx, dy, 0.0f);        // �߽� ����
        glRotatef(angle, 0, 0, 1);         // �߽��� ���ϵ���
        glScalef(1.2f, 0.9f, 1.0f);        // �׶��� Ÿ����
        glColor3f(1.0f, 1.0f, 1.0f);       // White
        glutSolidSphere(0.13, 20, 20);     // ���� �ϳ�
        glPopMatrix();
    }

    // �߽� ��ü
    glColor3f(1.0f, 1.0f, 0.2f);
    glutSolidSphere(0.08, 20, 20);        // �߽� ��ü

    glPopMatrix();
}

// ŰƼ �� + �� ��� 
void drawKittyFace()
{
    // �� (Ÿ����)
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, plushTexture);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(1.4f, 1.4f, 1.4f);
    //glRotatef(90, 1, 0, 0); 
    glRotatef(180, 0, 1, 0);
    glScalef(1.2f, 1.0f, 1.0f);

    GLUquadric* faceQuad = gluNewQuadric();
    gluQuadricTexture(faceQuad, GL_TRUE);
    gluSphere(faceQuad, 0.95, 50, 50);
    gluDeleteQuadric(faceQuad);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // �� - ����
    glPushMatrix();
    glTranslatef(-0.63f, 0.57f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glRotatef(-37, 0, 1, 0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, plushTexture);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glColor3f(1.0f, 1.0f, 1.0f);

    GLUquadric* leftEar = gluNewQuadric();
    gluQuadricTexture(leftEar, GL_TRUE);
    gluCylinder(leftEar, 0.55, 0.0, 0.6, 20, 20); // ����
    gluDisk(leftEar, 0.0, 0.55, 20, 1);           // �ظ�
    gluDeleteQuadric(leftEar);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // �� - ������
    glPushMatrix();
    glTranslatef(0.63f, 0.57f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glRotatef(37, 0, 1, 0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, plushTexture);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glColor3f(1.0f, 1.0f, 1.0f);

    GLUquadric* rightEar = gluNewQuadric();
    gluQuadricTexture(rightEar, GL_TRUE);
    gluCylinder(rightEar, 0.55, 0.0, 0.6, 20, 20);
    gluDisk(rightEar, 0.0, 0.55, 20, 1);
    gluDeleteQuadric(rightEar);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // �� (����)
    glPushMatrix();
    glColor3f(0.35f, 0.23f, 0.12f);
    glTranslatef(-0.4f, 0.2f, 0.9f);
    glScalef(0.08f, 0.10f, 0.05f);
    glutSolidSphere(1.0, 30, 30);
    glPopMatrix();

    // �� (������)
    glPushMatrix();
    glColor3f(0.35f, 0.23f, 0.12f);
    glTranslatef(0.4f, 0.2f, 0.9f);
    glScalef(0.08f, 0.10f, 0.05f);
    glutSolidSphere(1.0, 30, 30);
    glPopMatrix();

    // �� (��� Ÿ��)
    glPushMatrix();
    glColor3f(1.0f, 0.9f, 0.1f);
    glTranslatef(0.0f, -0.06f, 0.95f);
    glScalef(0.18f, 0.12f, 0.05f);
    glutSolidSphere(0.7, 30, 30);
    glPopMatrix();

    // ���� - ����
    glColor3f(0.35f, 0.23f, 0.12f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex3f(-0.65f, 0.2f, 0.75f); glVertex3f(-1.1f, 0.3f, 0.75f);
    glVertex3f(-0.65f, 0.1f, 0.75f); glVertex3f(-1.1f, 0.1f, 0.75f);
    glVertex3f(-0.65f, 0.0f, 0.75f); glVertex3f(-1.1f, -0.1f, 0.75f);
    glEnd();

    // ���� - ������
    glColor3f(0.35f, 0.23f, 0.12f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex3f(0.65f, 0.2f, 0.75f); glVertex3f(1.1f, 0.3f, 0.75f);
    glVertex3f(0.65f, 0.1f, 0.75f); glVertex3f(1.1f, 0.1f, 0.75f);
    glVertex3f(0.65f, 0.0f, 0.75f); glVertex3f(1.1f, -0.2f, 0.75f);
    glEnd();

    // �� ���
    drawRedFlower();
    drawWhiteFlower(0.12f, 0.88f, 0.43f, -60, 0, -70);
    drawWhiteFlower(-0.2f, 0.88f, 0.43f, -60, 0, -46);
    drawWhiteFlower(-0.60f, 0.76f, 0.58f, -40, -25, 10);
}

void drawPinkShell() {
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();

    for (int i = 0; i < 5; i++) {
        float angle = i * 72.0f;
        float rad = angle * 3.14159f / 180.0f;
        float dx = 0.1f * cos(rad);
        float dy = 0.1f * sin(rad);

        glPushMatrix();
        glTranslatef(dx, dy, 0.0f);
        glRotatef(angle, 0, 0, 1);
        glScalef(1.2f, 0.8f, 1.0f);
        glColor3f(1.0f, 0.6f, 0.8f);  // ����ũ
        glutSolidSphere(0.07, 20, 20);
        glPopMatrix();
    }

    // �ٴ��� �ڱ� 3��
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    float stitchHeight = 0.12f;
    float stitchWidth = 0.03f;
    float stitchOffsets[3] = { -0.07f, 0.0f, 0.07f };
    float stitchAngles[3] = { 20.0f, 0.0f, -20.0f };

    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glTranslatef(stitchOffsets[i], -0.03f, 0.07f);
        glRotatef(stitchAngles[i], 0, 0, 1);
        glScalef(stitchWidth, stitchHeight, 0.01f);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    glPopMatrix();
}

// ���� �Լ�
void drawShellTop() {
    // ���� ����
    glPushMatrix();
    glTranslatef(-0.25f, -1.15f, 0.68f);
    drawPinkShell(); // �ؽ�ó ����
    glPopMatrix();

    // ��� ���� ���� (�ؽ�ó ����)
    glDisable(GL_TEXTURE_2D);  // �ؽ�ó ��
    glColor3f(1.0f, 0.6f, 0.8f); // ����ȫ��
    glLineWidth(6.0f);
    glBegin(GL_LINES);
    glVertex3f(-0.15f, -1.15f, 0.70f);
    glVertex3f(0.15f, -1.15f, 0.70f);
    glEnd();

    // ������ ����
    glPushMatrix();
    glTranslatef(0.25f, -1.15f, 0.68f);
    drawPinkShell();
    glPopMatrix();

    // ���� ���� �� (������) - �ؽ�ó ����
    glPushMatrix();
    glTranslatef(0.0f, -1.15f, 0.0f);
    glRotatef(90, 1, 0, 0);
    glColor3f(1.0f, 0.6f, 0.8f); // ����ȫ ����

    glutSolidTorus(0.03f, 0.70f, 20, 60); // �ؽ�ó ���� ����
    glPopMatrix();
}

// ����
void createBodyCylinder(GLUquadric* quad) {
    float bottomRadius = 0.8f;   // �Ʒ��� �� ����
    float topRadius = 0.5f;      // ���� ����
    float height = 1.3f;

    glPushMatrix();

    // ��ġ ���� (�� �Ʒ�������)
    glTranslatef(0.0f, -1.6f, 0.0f);
    glRotatef(270, 1, 0, 0);

    // ���� ����
    glColor3f(0.9f, 0.75f, 0.6f);

    // ����� ���� (��ٸ���)
    //gluCylinder(quad, bottomRadius, topRadius, height, 30, 30);
    // ���� ���� �ؽ�ó ����
    glEnable(GL_TEXTURE_2D); // �ؽ�ó ��� �ѱ�
    glBindTexture(GL_TEXTURE_2D, plushTexture); // �ؽ�ó ���ε�
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glColor3f(1.0f, 1.0f, 1.0f);
    gluQuadricTexture(quad, GL_TRUE); // quadric ��ü�� �ؽ�ó Ȱ��ȭ

    gluCylinder(quad, bottomRadius, topRadius, height, 30, 30);

    // �ظ�, ���鿡�� �ؽ�ó ���� ����
    glPushMatrix();
    gluDisk(quad, 0.0f, bottomRadius, 30, 1);
    glTranslatef(0.0f, 0.0f, height);
    gluDisk(quad, 0.0f, topRadius, 30, 1);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D); // ������ ����

    // �ظ� �ݱ�
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    gluDisk(quad, 0.0f, bottomRadius, 30, 1); // �ظ�
    glPopMatrix();

    // ���� �ݱ�
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, height);
    gluDisk(quad, 0.0f, topRadius, 30, 1); // ����
    glPopMatrix();

    glPopMatrix();
}

// �ٸ�
void createLegCylinder(GLUquadric* quad, float xOffset) {
    float radius = 0.38f;
    float height = 0.4f;

    glPushMatrix();

    // ��ġ: ���� ���� �Ʒ��� + �¿�� xOffset ��ŭ �̵�
    glTranslatef(xOffset, -2.0f, 0.0f);  // y���� ���� �ؿ� ���� ����
    glRotatef(270, 1, 0, 0);             // ������ �ٸ�ó�� ���̰�

    glColor3f(0.9f, 0.75f, 0.6f);        // �� ���� ����

    glEnable(GL_TEXTURE_2D); // �ؽ�ó ��� �ѱ�
    glBindTexture(GL_TEXTURE_2D, plushTexture); // �ؽ�ó ���ε�
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glColor3f(1.0f, 1.0f, 1.0f);
    gluQuadricTexture(quad, GL_TRUE); // quadric ��ü�� �ؽ�ó Ȱ��ȭ

    gluCylinder(quad, radius, radius, height, 20, 20); // �ٸ� �����

    // �ظ� �ݱ�
    glPushMatrix();
    gluDisk(quad, 0.0f, radius, 20, 1);
    glPopMatrix();

    // ���� �ݱ�
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, height);
    gluDisk(quad, 0.0f, radius, 20, 1);
    glPopMatrix();

    glPopMatrix();
}

void drawLegs(GLUquadric* quad) {
    createLegCylinder(quad, -0.42f);  // ���� �ٸ�
    createLegCylinder(quad, 0.42f);  // ������ �ٸ�
}

// ġ��
void drawSkirtBelt() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, feltTexture);  // ������ �ؽ�ó
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(1.0f, 1.0f, 1.0f);  // �ؽ�ó �� �״��

    float innerRadius = 0.06f;
    float outerRadius = 0.85f;
    int sides = 20;
    int rings = 60;

    glPushMatrix();
    glTranslatef(0.0f, -1.38f, 0.0f);
    glRotatef(90, 1, 0, 0);  // ������ ����

    for (int i = 0; i < sides; ++i) {
        float theta = (float)i / sides * 2.0f * (float)M_PI;
        float nextTheta = (float)(i + 1) / sides * 2.0f * (float)M_PI;

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= rings; ++j) {
            float phi = (float)j / rings * 2.0f * (float)M_PI;

            // ���� �� �ѷ��� ��ǥ
            float x0 = (outerRadius + innerRadius * cos(theta)) * cos(phi);
            float y0 = (outerRadius + innerRadius * cos(theta)) * sin(phi);
            float z0 = innerRadius * sin(theta);

            // ���� �� �ѷ��� ��ǥ
            float x1 = (outerRadius + innerRadius * cos(nextTheta)) * cos(phi);
            float y1 = (outerRadius + innerRadius * cos(nextTheta)) * sin(phi);
            float z1 = innerRadius * sin(nextTheta);

            // �ؽ�ó ��ǥ (u, v)
            float u = (float)j / rings;
            float v0 = (float)i / sides;
            float v1 = (float)(i + 1) / sides;

            glTexCoord2f(u, v0); glVertex3f(x0, y0, z0);
            glTexCoord2f(u, v1); glVertex3f(x1, y1, z1);
        }
        glEnd();
    }

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void drawGrassSkirt() {
    int count = 20;
    float angleStep = 360.0f / count;
    float radius = 0.85f;
    float height = 0.35f;
    float width = 0.18f;
    float depth = 0.03f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, feltTexture);  // �ؽ�ó ���ε�
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(1.0f, 1.0f, 1.0f);

    for (int i = 0; i < count; ++i) {
        float angle = i * angleStep;
        float rad = angle * 3.14159f / 180.0f;
        float x = radius * cos(rad);
        float z = radius * sin(rad);

        glPushMatrix();
        glTranslatef(x, -1.73f, z);
        glRotatef(180, 0, 1, 0);  // ������ ���ϵ��� ȸ��

        float w = width / 2, h = height, d = depth / 2;

        glBegin(GL_QUADS);

        // �ո�
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0.0f, d);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0.0f, d);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, d);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, d);

        // �޸�
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0.0f, -d);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0.0f, -d);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, -d);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, -d);

        // ����
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, 0.0f, -d);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-w, 0.0f, d);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-w, h, d);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, -d);

        // ����
        glTexCoord2f(0.0f, 0.0f); glVertex3f(w, 0.0f, -d);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(w, 0.0f, d);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, d);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(w, h, -d);

        // ����
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-w, h, -d);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(w, h, -d);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(w, h, d);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-w, h, d);

        glEnd();
        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D);
}

// HAWAII �۾� �߰�
void drawHawaiiTextOnSkirt() {
    glPushMatrix();

    // ��ġ: ġ�� ���� ���� ��ó�� �̵�
    glTranslatef(-0.47f, -1.72f, 0.71f); // z: ��¦ ��
    glRotatef(90, 0, 0, 1);             // �������� 90�� ȸ���Ͽ� ����
    glScalef(0.0007f, 0.0007f, 0.7f);   // �ؽ�Ʈ ũ�� ����

    glColor3f(1.0f, 0.6f, 0.8f);        // ����ȫ �ؽ�Ʈ
    glLineWidth(1.0f);

    const char* text = "HAWAII";
    for (int i = 0; text[i] != '\0'; i++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
    }

    glPopMatrix();
}

// ����
void createLeftArm(GLUquadric* quad) {
    float bottomRadius = 0.28f;
    float topRadius = 0.1f;
    float height = 0.8f;

    glPushMatrix();

    // �� ��ġ �� ȸ��
    glTranslatef(-0.8f, -1.4f, 0.0f);
    glRotatef(270, 1, 0, 0);
    glRotatef(30, 0, 1, 0);

    // �� ����
    glColor3f(0.9f, 0.75f, 0.6f);

    glEnable(GL_TEXTURE_2D); // �ؽ�ó ��� �ѱ�
    glBindTexture(GL_TEXTURE_2D, plushTexture); // �ؽ�ó ���ε�
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glColor3f(1.0f, 1.0f, 1.0f);
    gluQuadricTexture(quad, GL_TRUE); // quadric ��ü�� �ؽ�ó Ȱ��ȭ

    // �� �����
    gluCylinder(quad, bottomRadius, topRadius, height, 30, 30);

    // �ظ�
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    gluDisk(quad, 0.0f, bottomRadius, 30, 1);
    glPopMatrix();

    // ����
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, height);
    gluDisk(quad, 0.0f, topRadius, 30, 1);
    glPopMatrix();

    // �� ��ü: �� ���ʿ� �����̱�
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);

    GLUquadric* handSphere = gluNewQuadric();
    gluQuadricTexture(handSphere, GL_TRUE);
    gluSphere(handSphere, 0.28, 30, 30);
    gluDeleteQuadric(handSphere);
    glPopMatrix();

    glPopMatrix(); // ��ü �� + �� ��
}

// ������
void createRightArm(GLUquadric* quad) {
    float bottomRadius = 0.28f;
    float topRadius = 0.25f;
    float height = 0.8f;

    glPushMatrix();

    // �� ��ġ �� ȸ��
    glTranslatef(1.1f, -0.92f, 0.0f);
    glRotatef(90, 1, 0, 0);
    glRotatef(-72, 0, 1, 0);

    glEnable(GL_TEXTURE_2D); // �ؽ�ó ��� �ѱ�
    glBindTexture(GL_TEXTURE_2D, plushTexture); // �ؽ�ó ���ε�
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glColor3f(1.0f, 1.0f, 1.0f);
    gluQuadricTexture(quad, GL_TRUE); // quadric ��ü�� �ؽ�ó Ȱ��ȭ

    // �� �����
    gluCylinder(quad, bottomRadius, topRadius, height, 30, 30);

    // �ظ�
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    gluDisk(quad, 0.0f, bottomRadius, 30, 1);
    glPopMatrix();

    // ����
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, height);
    gluDisk(quad, 0.0f, topRadius, 30, 1);
    glPopMatrix();

    // �� ��ü
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    GLUquadric* handSphere = gluNewQuadric();
    gluQuadricTexture(handSphere, GL_TRUE);
    gluSphere(handSphere, 0.28, 30, 30);
    gluDeleteQuadric(handSphere);
    glPopMatrix();

    // ���� �հ��� ��ü
    glPushMatrix();
    glTranslatef(-0.27f, 0.0f, -0.03f);
    GLUquadric* fingerSphere = gluNewQuadric();
    gluQuadricTexture(fingerSphere, GL_TRUE);
    gluSphere(fingerSphere, 0.1f, 30, 30);
    gluDeleteQuadric(fingerSphere);
    glPopMatrix();

    glPopMatrix(); // ��ü �� + �� ��
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0, 0, zoomZ, 0, 0, 0, 0, 1, 0);

    glRotatef(angleY, 1.0, 0.0, 0.0);
    glRotatef(angleX, 0.0, 1.0, 0.0);

    drawKittyFace();

    GLUquadric* quad = gluNewQuadric();
    createBodyCylinder(quad);
    drawLegs(quad);
    createLeftArm(quad);
    createRightArm(quad);
    gluDeleteQuadric(quad);
    drawShellTop();
    drawSkirtBelt();
    drawGrassSkirt();
    drawHawaiiTextOnSkirt();

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    if (h == 0) h = 1;
    float ratio = 1.0f * w / h;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, ratio, 1, 100);

    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isDragging = true;
        lastX = x;
        lastY = y;
    }
    if (state == GLUT_UP) {
        isDragging = false;
    }
}

void motion(int x, int y)
{
    if (isDragging) {
        angleX += (x - lastX);
        angleY += (y - lastY);
        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}

void mouseWheel(int button, int dir, int x, int y)
{
    if (dir > 0)
        zoomZ -= 0.2f; // zoom in
    else
        zoomZ += 0.2f; // zoom out

    if (zoomZ < 1.5f) zoomZ = 1.5f;
    if (zoomZ > 20.0f) zoomZ = 20.0f;

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'a':  // �� ��
    case 'A':
        zoomZ -= 0.2f;
        if (zoomZ < 1.5f) zoomZ = 1.5f;
        glutPostRedisplay();
        break;
    case 'z':  // �� �ƿ�
    case 'Z':
        zoomZ += 0.2f;
        if (zoomZ > 20.0f) zoomZ = 20.0f;
        glutPostRedisplay();
        break;
    }
}

void init()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.7f, 0.9f, 1.0f, 1.0f);  // ����
    loadTexture("Texture/plush_texture.png", &plushTexture);
    loadTexture("Texture/felt_texture.png", &feltTexture);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("3D Tanning Kitty");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);  // Ű���� �Է� ���

    glutMainLoop();
    return 0;
}

// �ؽ�ó �ε� �Լ�
void loadTexture(const char* filename, GLuint* textureID) {
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (!image) {
        printf("Failed to load texture: %s\n", filename);
        exit(1);
    }

    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, *textureID);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(image);
}