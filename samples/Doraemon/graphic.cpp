/*
 *		Computer Graphics Project
 *		This Code Make 3D Doraemon.
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ====== 전역 변수 ======
int viewport = 0;
GLfloat gYAngle = 0, gXAngle = 0, gZAngle = 0;
GLint moveX = 0, moveY = 0;
GLuint texture[2];
GLUquadricObj *body;
GLUquadricObj *head;

// ====== 함수 선언 ======
void myLight(void);
void myDisplay(void);
void myKeyboard(unsigned char key, int x, int y);
void myMouseMotion(GLint X, GLint Y);
int loadGLTextures(void);

// ====== stb_image로 BMP 로딩 + 텍스처 생성 ======
static int loadTexture(const char *filename, GLuint tex)
{
	int w, h, channels;
	unsigned char *data = stbi_load(filename, &w, &h, &channels, 3);
	if (!data) {
		std::fprintf(stderr, "텍스처 로딩 실패: %s\n", filename);
		return 0;
	}

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
	return 1;
}

int loadGLTextures()
{
	head = gluNewQuadric();
	gluQuadricDrawStyle(head, GLU_FILL);
	gluQuadricTexture(head, GL_TRUE);
	body = gluNewQuadric();
	gluQuadricDrawStyle(body, GLU_FILL);
	gluQuadricTexture(body, GL_TRUE);

	glGenTextures(2, texture);

	int status = 1;
	status &= loadTexture("head.bmp", texture[0]);
	status &= loadTexture("body.bmp", texture[1]);

	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	return status;
}

// ====== 조명 설정 ======
void myLight()
{
	float AmbientColor[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
	float DiffuseColor[]  = { 0.8f, 0.8f, 0.8f, 1.0f };
	float SpecularColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float EmissionColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float Position[]      = { 100.0f, 100.0f, 400.0f, 1.0f };

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_LIGHTING);

	glMaterialfv(GL_FRONT, GL_AMBIENT, AmbientColor);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, DiffuseColor);
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	glLightfv(GL_LIGHT0, GL_EMISSION, EmissionColor);
	glLightfv(GL_LIGHT0, GL_POSITION, Position);

	(void)SpecularColor; // 원본에서 미사용
	glEnable(GL_LIGHT0);
}

// ====== 렌더링 ======
void myDisplay()
{
	GLfloat d1[] = { 0.0f, 0.5f, 0.83f, 1.0f };
	GLfloat d2[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat d3[] = { 0.7f, 0.0f, 0.0f, 1.0f };
	(void)d1; (void)d3; // 원본에서 미사용

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0 + viewport, 0 + viewport, 600 - 2 * viewport, 600 - 2 * viewport);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// 마우스 드래그 회전
	glRotatef(gYAngle, 0.0f, 1.0f, 0.0f);
	glRotatef(gXAngle, 1.0f, 0.0f, 1.0f);
	glRotatef(gZAngle, 0.0f, 0.0f, 0.0f);

	GLUquadricObj *obj = gluNewQuadric();
	gluQuadricDrawStyle(obj, GLU_FILL);
	gluQuadricNormals(obj, GL_SMOOTH);

	// 머리
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glRotatef(270.0f, 1.0f, 0.0f, 0.0f);
	gluSphere(head, 0.3, 50, 30);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[1]);

	glTranslatef(0.0f, -0.43f, 0.0f);
	glRotatef(270.0f, 1.0f, 0.0f, 0.0f);
	gluSphere(head, 0.209, 30, 20);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	// 목도리
	glMaterialfv(GL_FRONT, GL_DIFFUSE, d2);
	glTranslatef(0.0f, 0.15f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidTorus(0.025, 0.15, 30, 30);

	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glTranslatef(0.0f, -0.17f, 0.0f);

	// 왼쪽 다리
	glMaterialfv(GL_FRONT, GL_DIFFUSE, d2);
	glPushMatrix();
	glTranslatef(-0.09f, -0.077f, 0.0f);
	glRotatef(75.0f, 0.1f, 0.0f, 0.0f);
	glRotatef(355.0f, 0.0f, 1.0f, 0.0f);
	gluCylinder(obj, 0.1, 0.07, 0.2, 30, 10);
	glPopMatrix();

	// 왼발
	glPushMatrix();
	glTranslatef(-0.1f, -0.28f, 0.06f);
	glRotatef(270.0f, 0.1f, 0.0f, 0.0f);
	glRotatef(5.0f, 0.0f, 1.0f, 0.0f);
	glutSolidTorus(0.03, 0.06, 30, 30);
	glPopMatrix();

	// 오른쪽 다리
	glPushMatrix();
	glTranslatef(0.09f, -0.075f, 0.0f);
	glRotatef(75.0f, 0.1f, 0.0f, 0.0f);
	glRotatef(3.0f, 0.0f, 0.1f, 0.0f);
	gluCylinder(obj, 0.1, 0.07, 0.2, 30, 30);
	glPopMatrix();

	// 오른발
	glPushMatrix();
	glTranslatef(0.1f, -0.28f, 0.06f);
	glRotatef(90.0f, 0.1f, 0.0f, 0.0f);
	glutSolidTorus(0.03, 0.06, 30, 10);
	glPopMatrix();

	// 오른쪽 팔
	glPushMatrix();
	glTranslatef(0.13f, 0.089f, 0.0f);
	glRotatef(90.0f, 0.1f, 0.2f, 0.0f);
	gluCylinder(obj, 0.07, 0.05, 0.2, 3, 3);
	// 오른손
	glTranslatef(0.0f, -0.007f, 0.2f);
	glutSolidSphere(0.05, 20, 20);
	glPopMatrix();

	// 왼쪽 팔
	glPushMatrix();
	glTranslatef(-0.13f, 0.089f, 0.0f);
	glRotatef(90.0f, 0.1f, -0.2f, 0.0f);
	gluCylinder(obj, 0.07, 0.05, 0.2, 3, 3);
	// 왼손
	glTranslatef(0.0f, -0.008f, 0.2f);
	glutSolidSphere(0.05, 20, 20);
	glPopMatrix();

	// 꼬리
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, -0.008f, 0.2f);
	glutSolidSphere(0.05, 20, 20);
	glPopMatrix();

	glutSwapBuffers();
	gluDeleteQuadric(obj);
}

// ====== 입력 처리 ======
void myKeyboard(unsigned char key, int x, int y)
{
	(void)x; (void)y;
	switch (key) {
	case 'z':
		viewport += 5;
		glutPostRedisplay();
		break;
	case 'a':
		viewport -= 5;
		glutPostRedisplay();
		break;
	case 'q':
		std::exit(0);
		break;
	}
}

void myMouseMotion(GLint x, GLint y)
{
	if (x - moveX > 0)
		gYAngle += 5.0f;
	else if (x - moveX < 0)
		gYAngle -= 5.0f;

	if (y - moveY > 0)
		gXAngle += 5.0f;
	else if (y - moveY < 0)
		gXAngle -= 5.0f;

	moveX = x;
	moveY = y;
	glutPostRedisplay();
}

// ====== main ======
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Doraemon");
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	myLight();
	loadGLTextures();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glutDisplayFunc(myDisplay);
	glutKeyboardFunc(myKeyboard);
	glutMotionFunc(myMouseMotion);
	glutMainLoop();
	return 0;
}
