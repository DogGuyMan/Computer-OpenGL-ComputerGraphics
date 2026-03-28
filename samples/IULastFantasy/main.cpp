#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TEX_BLACK 1
#define TEX_BODY 2
#define TEX_SKIN 3
#define TEX_ARM 4
#define TEX_SHOE 5
#define TEX_EYE 6
#define TEX_FACE 7
#define TEX_HAIR 8
#define PI 3.141592f
void createCircle(GLfloat, GLfloat, GLfloat);
void createCylinder(GLfloat,GLfloat,GLfloat);
void createCylinder_tex(GLfloat,GLfloat,GLfloat, int, int);
int LoadGLTextures(const char*,int);
void createSphere(GLfloat);

float x, y, z;
float radius;
float theta;
float phi;
float zoom = 60.0f;

int beforeX, beforeY;

GLuint texture[10];

void InitLight()
{
	GLfloat ambientLight[4];
    GLfloat diffuseLight[4];
    GLfloat lightPosition[4];
    GLfloat specular[4];
    GLfloat yrot;

	ambientLight[0] = 0.3f;
    ambientLight[1] = 0.3f;
    ambientLight[2] = 0.3f;
    ambientLight[3] = 1.0f;

    diffuseLight[0] = 0.7f;
    diffuseLight[1] = 0.7f;
    diffuseLight[2] = 0.7f;
    diffuseLight[3] = 1.0f;

    lightPosition[0] = 0.0f;
    lightPosition[1] = 0.0f;
    lightPosition[2] = 5.0f;
    lightPosition[3] = 1.0f;

    specular[0] = 1.0f;
    specular[1] = 1.0f;
    specular[2] = 1.0f;
    specular[3] = 1.0f;

    yrot = 0.0f;

    (void)yrot;

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glFrontFace(GL_CCW);

    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHT0);
}

int init (void)
{
	LoadGLTextures("Data/black.bmp",TEX_BLACK);
	LoadGLTextures("Data/body.bmp",TEX_BODY);
	LoadGLTextures("Data/skin.bmp",TEX_SKIN);
	LoadGLTextures("Data/arm.bmp",TEX_ARM);
	LoadGLTextures("Data/shoe.bmp",TEX_SHOE);
	LoadGLTextures("Data/eye.bmp",TEX_EYE);
	LoadGLTextures("Data/face.bmp",TEX_FACE);
	LoadGLTextures("Data/hair.bmp",TEX_HAIR);

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    return 1;
}

void reshape (int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(zoom, 1.0, 1.0, 100.0);

    radius = 10.0f;
    theta = 10.0f;
    phi = -10.0f;
}
void display (void)
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(zoom, 1.0, 1.0, 100.0);

    x = -radius * cosf(phi) * cosf(theta);
    y = -radius * cosf(phi) * sinf(theta);
    z = radius * sinf(phi);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);

    glBindTexture(GL_TEXTURE_2D, texture[TEX_ARM]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(2.5f, 0.0f, -0.5f);
	glRotatef(30, 0.0f, -1.0f, 0.0f);
    createCylinder_tex(0.4f, 0.4f, 2.0f,TEX_SKIN,TEX_BLACK); // right arm

    glBindTexture(GL_TEXTURE_2D, texture[TEX_ARM]);
    glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-1.5f, -2.0f, 1.0f);
	glRotatef(90.0f, -1.0f, 0.0f, 0.0f);
    createCylinder_tex(0.4f, 0.4f, 2.0f,TEX_SKIN,TEX_BLACK); // left arm

    glBindTexture(GL_TEXTURE_2D, texture[TEX_SKIN]);
    glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-0.8f, 0.0f, -3.2f);
    createCylinder(0.4f, 1.0f, 3.0f); // left leg

    glBindTexture(GL_TEXTURE_2D, texture[TEX_SKIN]);
    glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(0.8f, 0.0f, -3.2f);
    createCylinder(0.4f, 1.0f, 3.0f); // right leg

    glBindTexture(GL_TEXTURE_2D, texture[TEX_BODY]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(0.0f, 0.0f, -1.5f);
	createCylinder_tex(2.3f,1.3f,3.0f,TEX_BLACK,TEX_BLACK);   // body

	glBindTexture(GL_TEXTURE_2D, texture[TEX_SKIN]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-1.5f, -2.0f, 1.0f);
    createSphere(0.4f); // left hand

	glBindTexture(GL_TEXTURE_2D, texture[TEX_SKIN]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-1.4f, -2.2f, 1.2f);
    createCylinder(0.1f, 0.1f, 0.4f); // left finger

	glBindTexture(GL_TEXTURE_2D, texture[TEX_SKIN]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-1.4f, -2.2f, 1.6f);
    createSphere(0.1f); // left finger point

	glBindTexture(GL_TEXTURE_2D, texture[TEX_SKIN]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-1.05f, -2.2f, 1.0f);
	glRotatef(90.0f, 0.0f, -1.0f, 0.0f);
    createCylinder(0.1f, 0.1f, 0.4f); // left finger2

	glBindTexture(GL_TEXTURE_2D, texture[TEX_SKIN]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-1.05f, -2.2f, 1.0f);
    createSphere(0.1f); // left finger2 point

	glBindTexture(GL_TEXTURE_2D, texture[TEX_SKIN]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(2.5f, 0.0f, -0.5f);
    createSphere(0.4f); // right hand

	glBindTexture(GL_TEXTURE_2D, texture[TEX_BLACK]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-1.5f, 0.0f, 1.0f);
    createSphere(0.4f); // left sholder

	glBindTexture(GL_TEXTURE_2D, texture[TEX_BLACK]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(1.5f, 0.0f, 1.2f);
    createSphere(0.4f); // right sholder

	glBindTexture(GL_TEXTURE_2D, texture[TEX_SHOE]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(0.8f, 0.0f, -3.2f);
    createSphere(0.4f); // right shoe

	glBindTexture(GL_TEXTURE_2D, texture[TEX_SHOE]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-0.8f, 0.0f, -3.2f);
    createSphere(0.4f); // left shoe

	glBindTexture(GL_TEXTURE_2D, texture[TEX_HAIR]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-0.5f, -0.1f, 3.6f);
	glRotatef(50, 0.0f, -1.0f, 0.0f);
	glScalef(1.0f, 1.0f, 0.8f);
    createSphere(1.8f); // left hair

	glBindTexture(GL_TEXTURE_2D, texture[TEX_HAIR]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(0.5f, -0.1f, 3.6f);
	glRotatef(-50, 0.0f, -1.0f, 0.0f);
	glScalef(1.0f, 1.0f, 0.8f);
    createSphere(1.8f); // right hair

	glBindTexture(GL_TEXTURE_2D, texture[TEX_FACE]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(0.0f, 0.0f, 3.0f);
	glScalef(0.9f, 1.0f, 1.0f);
    createSphere(2.0f); // head

	glBindTexture(GL_TEXTURE_2D, texture[TEX_HAIR]);
	glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(0.0f, 0.3f, 3.2f);
	glRotatef(-180, 0.0f, 0.0f, 1.0f);
    createSphere(2.0f); // back hair

	glBindTexture(GL_TEXTURE_2D, texture[TEX_HAIR]);
    glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(2.0f, 0.6f, 2.2f);
	glRotatef(-60, 0.0f, -1.0f, 0.0f);
	glScalef(4.0f, 1.2f, 1.3f);
    createSphere(0.4f); // right pony

	glBindTexture(GL_TEXTURE_2D, texture[TEX_HAIR]);
    glLoadIdentity();
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glTranslatef(-2.0f, 0.6f, 2.2f);
	glRotatef(60, 0.0f, -1.0f, 0.0f);
	glScalef(4.0f, 1.2f, 1.3f);
    createSphere(0.4f); // left pony

    glFlush ();
    glutSwapBuffers();
}

// ZOOM
void keyboard( unsigned char key, int x, int y )
{
    (void)x; (void)y;
    switch(key)
    {
        case 'a':
            zoom--;
            break;
        case 'z':
            zoom++;
            break;
    }

}

// VIEWPOINT 1
void processMouse(int button, int state, int x, int y)
{

    if(button == GLUT_LEFT_BUTTON){
        if(state == GLUT_DOWN){
            beforeX = x;
            beforeY = y;
        }
    }
}

// VIEWPOINT 2
void processMouseMotion(int x, int y)
{
    if(abs(beforeX-x) > abs(beforeY-y)){
        if(beforeX < x)
        {
            theta -= 0.1f;
        }else if(beforeX > x){
            theta += 0.1f;
        }
    }else {
        if(beforeY > y){
            phi -= 0.1f;
        }else if(beforeY < y){
            phi -= 0.1f;
        }
    }

    beforeX = x;
    beforeY = y;

    glutPostRedisplay();

    if ( theta > 2.0f * PI )
        theta -= (2.0f * PI);
    else if ( theta < 0.0f )
        theta += (2.0f * PI);
}

void createSphere(GLfloat r)
{
GLUquadricObj *sphere = gluNewQuadric();

  gluQuadricTexture(sphere, GL_TRUE);
  gluQuadricDrawStyle(sphere, GLU_FILL);
  glPolygonMode(GL_FRONT, GL_FILL);
  gluQuadricNormals(sphere, GLU_SMOOTH);
  gluSphere(sphere,r,36 ,18);
}

void createCylinder_tex(GLfloat bottom, GLfloat top, GLfloat height, int texture1, int texture2)
{
  GLUquadricObj *cylinder = gluNewQuadric();

  gluQuadricTexture(cylinder, GL_TRUE);
  gluQuadricDrawStyle(cylinder, GLU_FILL);
  glPolygonMode(GL_FRONT, GL_FILL);
  gluQuadricNormals(cylinder, GLU_SMOOTH);
  gluCylinder(cylinder, bottom, top, height, 20, 100);

  glBindTexture(GL_TEXTURE_2D, texture[texture1]);
  createCircle(bottom, 1.0f, 0.0f);
  glBindTexture(GL_TEXTURE_2D, texture[texture2]);
  createCircle(top, -1.0f, height);
}

void createCylinder(GLfloat bottom, GLfloat top, GLfloat height)
{
  GLUquadricObj *cylinder = gluNewQuadric();

  gluQuadricTexture(cylinder, GL_TRUE);
  gluQuadricDrawStyle(cylinder, GLU_FILL);
  glPolygonMode(GL_FRONT, GL_FILL);
  gluQuadricNormals(cylinder, GLU_SMOOTH);
  gluCylinder(cylinder, bottom, top, height, 20, 100);

  createCircle(bottom, 1.0f, 0.0f);
  createCircle(top, -1.0f, height);
}

// for cylinder top & bottom
void createCircle(GLfloat r, GLfloat pos, GLfloat move){
  GLfloat centerx = 0, centery = 0, centerz = 0;
  GLfloat xx, yy, angle;

  glBegin(GL_TRIANGLE_FAN);
  glTexCoord2f(0.0f, 0.0f);
  glNormal3f(0.0f, 0.0f, pos);
  glVertex3f(centerx, centery, centerz + move);
  for(angle = (2.0f*PI); angle > 0.0f; angle -= (PI/8.0f))
  {
      xx = centerx + r*sinf(angle);
      yy = centery + r*cosf(angle);
	  glTexCoord2f(xx, yy);
      glNormal3f(0.0f, 0.0f, pos);
      glVertex3f(xx, yy, centerz + move);
  }
  glEnd();
}

// stb_image로 BMP 텍스처 로딩
int LoadGLTextures(const char* filename, int num)
{
	int w, h, channels;
	stbi_set_flip_vertically_on_load(1); // BMP는 bottom-to-top, OpenGL도 동일 순서 기대
	unsigned char *data = stbi_load(filename, &w, &h, &channels, 3);
	if (!data) {
		std::fprintf(stderr, "텍스처 로딩 실패: %s\n", filename);
		return 0;
	}

	glGenTextures(1, &texture[num]);
	glBindTexture(GL_TEXTURE_2D, texture[num]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	stbi_image_free(data);
	return 1;
}

int main (int argc, char** argv)
{
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowPosition (100, 100);
    glutInitWindowSize (1000, 1000);
    glutCreateWindow ("IU YOU&I ver");
    init();
    InitLight();
    glutDisplayFunc (display);
    glutReshapeFunc (reshape);
    glutMouseFunc(processMouse);
    glutMotionFunc(processMouseMotion);
	glutKeyboardFunc(keyboard);
    glutIdleFunc(display);
    glutMainLoop();
    return 0;
}
