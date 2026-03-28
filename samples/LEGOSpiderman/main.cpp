// ====== 크로스플랫폼 OpenGL 헤더 ======
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// stb_image: glaux 대체 (크로스플랫폼 이미지 로딩)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PI_ 3.14159265358979323846

GLuint theTorus;
typedef float vec3_t[3];

// 텍스처 식별자 3개 (1=앞면, 2=뒷면, 3=머리)
GLuint texture[3];

// stb_image로 BMP 로딩 + OpenGL 텍스처 생성
static int LoadGLTextures()
{
	const char *files[3] = {"./image/front.bmp", "./image/back.bmp", "./image/head.bmp"};

	glGenTextures(3, texture);

	for (int i = 0; i < 3; i++) {
		int w, h, channels;
		unsigned char *data = stbi_load(files[i], &w, &h, &channels, 3); // RGB 강제
		if (!data) {
			std::fprintf(stderr, "텍스처 로딩 실패: %s\n", files[i]);
			return 0;
		}

		glBindTexture(GL_TEXTURE_2D, texture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}

	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	return 1;
}

vec3_t color[] = {
	{1.0f, 0.0f, 0.0f},  // red
	{0.0f, 1.0f, 0.0f},  // green
	{0.0f, 0.0f, 1.0f},  // blue
	{1.0f, 1.0f, 0.0f},  // yellow
	{1.0f, 0.0f, 1.0f},  // magenta
	{0.0f, 1.0f, 1.0f},  // cyan
	{1.0f, 1.0f, 1.0f},  // white
	{.25f, .25f, .25f},   // dark gray
	{.60f, .40f, .70f},   // barney purple
	{.98f, .625f, .12f},  // pumpkin orange
	{.98f, .04f, .70f},   // pastel pink
	{.75f, .75f, .75f},   // light gray
	{.60f, .40f, .12f}    // brown
};
vec3_t rot = {0.f, 0.f, 0.f};
double eye[3] = {0., 0., 0.0001};
double center[3] = {0., 0., 0.};

double size = 3.;
float theta = 0.f;
float thetaDelta = .125f;
float eyeDelta = .125f;
float scale = 0.6f;
float scaleDelta = 1.05f;
float anitheta = 0.f;
float anithetaDelta = .125f;
GLUquadricObj *c;

int mouseX = 0;
int mouseY = 0;
int mouseState = 0;
int mouseButton = 0;
int projection = 0;
int aniOn = 0;
int depthOn = 1;
int openOn = 0;
int fillOn = 0;
int windowWidth, windowHeight;


static void torus(int numc, int numt)
{
	int i, j, k;
	double s, t, x, y, z, twopi;
	twopi = 2 * PI_;

	float col[3] = {0.0f, 0.1f, 1.0f};
	glColor3fv(col);

	for (i = 0; i < numc; i++) {
		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= (numt * 1) / 5; j++) {
			for (k = 1; k >= 0; k--) {
				s = (i + k) % numc + 0.5;
				t = j % numt;
				x = (1 + .2 * cos(s * twopi / numc)) * cos(t * twopi / numt);
				y = (1 + .2 * cos(s * twopi / numc)) * sin(t * twopi / numt);
				z = .3 * sin(s * twopi / numc);
				glVertex3d(x, y, z);
			}
		}
		glEnd();
	}
}
static void torus_hand(int numc, int numt)
{
	int i, j, k;
	double s, t, x, y, z, twopi;
	twopi = 2 * PI_;

	float col[3] = {1.0f, 0.0f, 0.0f};
	glColor3fv(col);

	for (i = 0; i < numc; i++) {
		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= (numt * 3) / 4; j++) {
			for (k = 1; k >= 0; k--) {
				s = (i + k) % numc + 0.5;
				t = j % numt;
				x = (1 + .3 * cos(s * twopi / numc)) * cos(t * twopi / numt);
				y = (1 + .3 * cos(s * twopi / numc)) * sin(t * twopi / numt);
				z = .3 * sin(s * twopi / numc);
				glVertex3d(x, y, z);
			}
		}
		glEnd();
	}
}
static void torus_hat(int numc, int numt)
{
	int i, j, k;
	double s, t, x, y, z, twopi;
	twopi = 2 * PI_;

	float col[3] = {1.0f, 0.0f, 0.0f};
	glColor3fv(col);

	for (i = 0; i < numc; i++) {
		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= numt; j++) {
			for (k = 1; k >= 0; k--) {
				s = (i + k) % numc + 0.5;
				t = j % numt;
				x = (1 + .1 * cos(s * twopi / numc)) * cos(t * twopi / numt);
				y = (1 + .1 * cos(s * twopi / numc)) * sin(t * twopi / numt);
				z = .4 * sin(s * twopi / numc);
				glVertex3d(x, y, z);
			}
		}
		glEnd();
	}
}

static void Leg()
{
	glBegin(GL_QUADS);
	// Top face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 0.5f);
	glVertex3f(-1.0f, -1.0f, -0.7f);
	glVertex3f(-0.1f, -1.0f, -0.7f);
	glVertex3f(-0.1f, -1.0f, 0.5f);

	// Bottom face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -3.0f, 0.5f);
	glVertex3f(-1.0f, -3.0f, -0.7f);
	glVertex3f(-0.1f, -3.0f, -0.7f);
	glVertex3f(-0.1f, -3.0f, 0.5f);

	// Front face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 0.5f);
	glVertex3f(-0.1f, -1.0f, 0.5f);
	glVertex3f(-0.1f, -3.0f, 0.5f);
	glVertex3f(-1.0f, -3.0f, 0.5f);

	// Back face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -0.7f);
	glVertex3f(-0.1f, -1.0f, -0.7f);
	glVertex3f(-0.1f, -3.0f, -0.7f);
	glVertex3f(-1.0f, -3.0f, -0.7f);

	// Left face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 0.5f);
	glVertex3f(-1.0f, -1.0f, -0.7f);
	glVertex3f(-1.0f, -3.0f, -0.7f);
	glVertex3f(-1.0f, -3.0f, 0.5f);

	// Right face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.1f, -1.0f, 0.5f);
	glVertex3f(-0.1f, -1.0f, -0.7f);
	glVertex3f(-0.1f, -3.0f, -0.7f);
	glVertex3f(-0.1f, -3.0f, 0.5f);
	glEnd();
}

static void foot()
{
	glBegin(GL_QUADS);
	// Top face
	glColor3f(0.2f, 0.2f, 0.8f);
	glVertex3f(-1.0f, -2.7f, 0.7f);
	glVertex3f(-1.0f, -2.7f, -0.7f);
	glVertex3f(-0.1f, -2.7f, -0.7f);
	glVertex3f(-0.1f, -2.7f, 0.7f);

	// Bottom face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -3.0f, 0.7f);
	glVertex3f(-1.0f, -3.0f, -0.7f);
	glVertex3f(-0.1f, -3.0f, -0.7f);
	glVertex3f(-0.1f, -3.0f, 0.7f);

	// Front face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -2.7f, 0.7f);
	glVertex3f(-0.1f, -2.7f, 0.7f);
	glVertex3f(-0.1f, -3.0f, 0.7f);
	glVertex3f(-1.0f, -3.0f, 0.7f);

	// Back face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -2.7f, -0.7f);
	glVertex3f(-0.1f, -2.7f, -0.7f);
	glVertex3f(-0.1f, -3.0f, -0.7f);
	glVertex3f(-1.0f, -3.0f, -0.7f);

	// Left face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -2.7f, 0.7f);
	glVertex3f(-1.0f, -2.7f, -0.7f);
	glVertex3f(-1.0f, -3.0f, -0.7f);
	glVertex3f(-1.0f, -3.0f, 0.7f);

	// Right face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.1f, -2.7f, 0.7f);
	glVertex3f(-0.1f, -2.7f, -0.7f);
	glVertex3f(-0.1f, -3.0f, -0.7f);
	glVertex3f(-0.1f, -3.0f, 0.7f);
	glEnd();
}

static void init(void)
{
	theTorus = glGenLists(1);
	glNewList(theTorus, GL_COMPILE);
	torus(100, 25);
	glEndList();
	glShadeModel(GL_FLAT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);
	if (!projection)
		gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], 0, 1, 0);

	glRotatef(rot[0], 1.0f, 0.0f, 0.0f);
	glRotatef(rot[1], 0.0f, 1.0f, 0.0f);
	glRotatef(rot[2], 0.0f, 0.0f, 1.0f);
	glScalef(scale, scale, scale);

	// 거미줄 (왼쪽)
	glPushMatrix();
	glColor3fv(color[0]);
	glRotatef(30.f, 0.f, 0.0f, 0.f);
	glRotatef(30.f, -1.0f, 0.0f, 0.f);
	glTranslatef(0.f, 0.f, 0.2f);
	glTranslatef(-.1f, -1.5f, 0.f);
	glScalef(2.f, 2.f, 2.f);
	torus(100, 25);
	glPopMatrix();

	// 거미줄 (오른쪽)
	glPushMatrix();
	glColor3fv(color[0]);
	glRotatef(30.f, 0.f, 0.0f, 0.f);
	glRotatef(30.f, -1.0f, 0.0f, 0.f);
	glTranslatef(0.f, 0.f, 0.3f);
	glTranslatef(-.1f, -1.5f, 0.f);
	glScalef(-2.f, 2.f, 2.f);
	torus(100, 25);
	glPopMatrix();

	// 왼쪽 다리
	glPushMatrix();
	glColor3fv(color[0]);
	Leg();
	glPopMatrix();

	// 오른쪽 다리
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(1.1f, 0.0f, 0.f);
	Leg();
	glPopMatrix();

	// 다리연결부분 - 실린더
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.f);
	glTranslatef(-0.99f, -1.3f, .4f);
	glRotatef(90.f, 0.0f, 1.0f, 0.f);
	gluQuadricDrawStyle(c, GLU_FILL);
	gluCylinder(c, 0.4, 0.4, 1.98, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.f);
	glRotatef(90.f, 1.0f, 0.0f, 0.f);
	glTranslatef(0.8f, 0.4f, 1.3f);
	gluQuadricDrawStyle(c, GLU_FILL);
	glutSolidSphere(0.4, 2, 10);
	glPopMatrix();
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.f);
	glRotatef(90.f, 1.0f, 0.0f, 0.f);
	glTranslatef(-0.8f, 0.4f, 1.3f);
	gluQuadricDrawStyle(c, GLU_FILL);
	glutSolidSphere(0.4, 2, 10);
	glPopMatrix();

	// 왼쪽 발
	glPushMatrix();
	glColor3fv(color[2]);
	foot();
	glPopMatrix();

	// 오른쪽 발
	glPushMatrix();
	glColor3fv(color[2]);
	glTranslatef(1.1f, 0.0f, 0.f);
	foot();
	glPopMatrix();

	// 목 - 실린더
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(0.f, 1.3f, 0.0f);
	glRotatef(90.f, 1.f, 0.f, 0.f);
	gluQuadricDrawStyle(c, GLU_FILL);
	gluCylinder(c, 0.5, 0.5, .3, 20, 20);
	glPopMatrix();

	// 팔1 - 실린더
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(1.6f, -0.7f, 0.7f);
	glRotatef(90.f, 3.2f, -1.8f, 3.4f);
	gluQuadricDrawStyle(c, GLU_FILL);
	gluCylinder(c, 0.25, 0.25, .5, 20, 20);
	glPopMatrix();
	// 팔1 - sphere
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(1.6f, -0.7f, 0.7f);
	glRotatef(90.f, 5.0f, -1.0f, -1.0f);
	glRotatef(90.f, 3.2f, -1.8f, 3.4f);
	gluQuadricDrawStyle(c, GLU_FILL);
	glutSolidSphere(0.26, 2, 10);
	glPopMatrix();

	// 손 - torus
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(1.6f, -1.3f, 1.03f);
	glRotatef(30.f, 0.0f, -0.1f, 0.f);
	glRotatef(90.f, 7.0f, -6.0f, -1.5f);
	glRotatef(90.f, 1.8f, -1.5f, 1.4f);
	glScalef(0.2f, 0.2f, 0.6f);
	torus_hand(100, 25);
	glPopMatrix();

	// 팔2 - 실린더
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(-1.8f, -0.7f, 0.7f);
	glRotatef(90.f, 3.2f, -1.8f, 3.4f);
	gluQuadricDrawStyle(c, GLU_FILL);
	gluCylinder(c, 0.25, 0.25, .5, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(-1.8f, -0.7f, 0.7f);
	glRotatef(90.f, 5.0f, -1.0f, -1.0f);
	glRotatef(90.f, 3.2f, -1.8f, 3.4f);
	gluQuadricDrawStyle(c, GLU_FILL);
	glutSolidSphere(0.26, 2, 10);
	glPopMatrix();

	// 손2 - torus
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(-1.75f, -1.35f, 1.03f);
	glRotatef(10.f, -1.0f, -0.1f, 0.f);
	glRotatef(90.f, 7.0f, -3.0f, -1.5f);
	glRotatef(90.f, 0.8f, -1.5f, 1.4f);
	glScalef(0.2f, 0.2f, 0.6f);
	torus_hand(100, 25);
	glPopMatrix();

	///////////////////////////////////////////////////////////
	// 머리 - 실린더 + 텍스처
	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(0.f, 2.3f, 0.0f);
	glRotatef(90.f, 1.f, 0.f, 0.f);
	gluQuadricDrawStyle(c, GLU_FILL);

	gluQuadricNormals(c, GLU_SMOOTH);
	gluQuadricTexture(c, GL_TRUE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[2]);

	glColor3f(1.0f, 1.0f, 1.0f);
	glRotatef(180.f, 1.f, 0.f, 0.f);
	glTranslatef(0.0f, 0.f, -1.1f);
	gluCylinder(c, 0.8, 0.8, 1.1, 20, 20);

	glPopMatrix();
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glTranslatef(0.f, 2.3f, 0.0f);
	glRotatef(90.f, 0.f, 0.f, 1.f);
	gluQuadricDrawStyle(c, GLU_FILL);
	glutSolidSphere(0.8, 2, 10); // 얼굴뚜껑
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.f, -1.1f, 0.0f);
	glTranslatef(0.f, 2.3f, 0.0f);
	glRotatef(90.f, 0.f, 0.f, 1.f);
	glutSolidSphere(0.8, 2, 10); // 얼굴뚜껑
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	// 머리둘레 (토러스)
	glPushMatrix();
	glColor3fv(color[0]);
	glTranslatef(0.0f, 2.35f, .03f);
	glRotatef(90.f, 1.0f, 0.0f, 0.0f);
	glScalef(0.4f, 0.4f, 0.6f);
	torus_hat(100, 25);
	glPopMatrix();

	// 앞면 (front 텍스처)
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.85f, 1.0f);
	glVertex3f(0.7f, 1.0f, 0.7f);

	glTexCoord2f(0.15f, 1.0f);
	glVertex3f(-0.7f, 1.0f, 0.7f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.7f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.7f);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	// 뒷면 (back 텍스처)
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -0.7f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -0.7f);

	glTexCoord2f(0.85f, 1.0f);
	glVertex3f(-0.7f, 1.0f, -0.7f);

	glTexCoord2f(0.15f, 1.0f);
	glVertex3f(0.7f, 1.0f, -0.7f);

	glEnd();
	glDisable(GL_TEXTURE_2D);

	// 몸통 나머지 면
	glBegin(GL_QUADS);
	// Top face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.7f, 1.0f, -0.7f);
	glVertex3f(-0.7f, 1.0f, -0.7f);
	glVertex3f(-0.7f, 1.0f, 0.7f);
	glVertex3f(0.7f, 1.0f, 0.7f);

	// Bottom face
	glColor3f(0.0f, 0.f, 0.8f);
	glVertex3f(1.0f, -1.0f, 0.7f);
	glVertex3f(-1.0f, -1.0f, 0.7f);
	glVertex3f(-1.0f, -1.0f, -0.7f);
	glVertex3f(1.0f, -1.0f, -0.7f);

	// Left face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.7f, 1.0f, 0.7f);
	glVertex3f(-0.7f, 1.0f, -0.7f);
	glVertex3f(-1.0f, -1.0f, -0.7f);
	glVertex3f(-1.0f, -1.0f, 0.7f);

	// Right face
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.7f, 1.0f, -0.7f);
	glVertex3f(0.7f, 1.0f, 0.7f);
	glVertex3f(1.0f, -1.0f, 0.7f);
	glVertex3f(1.0f, -1.0f, -0.7f);
	glEnd();

	glFlush();
	glutSwapBuffers();
}

void myResize(int width, int height)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (projection)
		glOrtho(-size, size, -size, size, -size, size);
	else
		gluPerspective(60., static_cast<double>(width) / height, .1, 100.);
	glEnable(GL_DEPTH_TEST);
	windowWidth = width;
	windowHeight = height;
}

void myIdle()
{
	anitheta += anithetaDelta;
	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
	(void)x;
	(void)y;
	switch (key) {
	case 'a':
		scale *= scaleDelta;
		break;
	case 'z':
		scale /= scaleDelta;
		break;
	}
	glutPostRedisplay();
}

void myMotion(int x, int y)
{
	if (mouseButton == GLUT_LEFT_BUTTON && mouseState == GLUT_DOWN) {
		rot[1] -= static_cast<float>(mouseX - x) / 2.f;
		rot[0] -= static_cast<float>(mouseY - y) / 2.f;
		glutPostRedisplay();
		mouseX = x;
		mouseY = y;
	}
}

void myInit()
{
	c = gluNewQuadric();
	gluQuadricDrawStyle(c, GLU_LINE);
}

void myMouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (aniOn) glutIdleFunc(nullptr);
		mouseState = state;
		mouseButton = btn;
		mouseX = x;
		mouseY = y;
	} else if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		mouseState = -1;
		if (aniOn) glutIdleFunc(myIdle);
	} else {
		return;
	}
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("LEGO Spiderman");
	myInit();
	init();
	LoadGLTextures();
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myResize);
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouse);
	glutMotionFunc(myMotion);
	glutIdleFunc(myIdle);
	glutMainLoop();
	return 0;
}
