/*
 *		Computer Graphics Project 
 *		20112407 �Ż���, 20082296 ��絵
 *		This Code Make 3D Doraemon.
 */
#pragma comment(lib, "glaux")
#include<Windows.h>
#include <iostream>
#include <stdio.h>
#include <GL/glaux.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>

void myLight(void);
void myDisplay(void);
void myKeyboard(unsigned char key, int x, int y);
//void myMouseClick(GLint Button, GLint State, GLint x, GLint y);
void myMouseMotion(GLint X, GLint Y);
int loadGLTextures(void);	
AUX_RGBImageRec *loadBMP(char *filename);

int viewport;
GLfloat gYAngle,gXAngle,gZAngle; 
GLint moveX,moveY;
GLuint texture[2];
GLUquadricObj *body;
GLUquadricObj *head; 

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (600, 600); 
	glutInitWindowPosition (0, 0);
	glutCreateWindow("���󿡸�"); 
	glClearColor (1.0, 1.0, 1.0, 1.0);
	myLight();
	loadGLTextures();
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glutDisplayFunc(myDisplay); 
	glutKeyboardFunc(myKeyboard);
	//glutMouseFunc(myMouseClick);
	glutMotionFunc(myMouseMotion);
	glutMainLoop( );
	return 0;
}

void myLight(){

	float AmbientColor[]    = { 0.2f, 0.2f, 0.2f, 1.0f };        //�ֺ���
	float DiffuseColor[]    = {  0.8f, 0.8f, 0.8f, 1.0f };        //�л걤
	float SpecularColor[]   = { 0.0f, 0.0f, 0.0f, 1.0f };        //��籤
	float EmissionColor[]   = { 1.0f, 1.0f, 1.0f, 1.0f };        //��ü
	float Position[]        = { 100.0f, 100.0f, 400.0f, 1.0f };  //���� ��ġ
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );                 
    glShadeModel( GL_SMOOTH );              
   
	glEnable(GL_TEXTURE_2D); 
    // ������ �� ����
    glEnable( GL_DEPTH_TEST );                              
    glEnable( GL_CULL_FACE );                              
    glFrontFace( GL_CCW );     
   
    // ���� Ȱ��ȭ
    glEnable( GL_LIGHTING );                                
  
    //������ ����
	glMaterialfv( GL_FRONT, GL_AMBIENT, AmbientColor );       
    glMaterialfv( GL_FRONT, GL_DIFFUSE, DiffuseColor );
    glLightfv( GL_LIGHT0, GL_AMBIENT, AmbientColor );       // �ֺ��� 
    glLightfv( GL_LIGHT0, GL_DIFFUSE, DiffuseColor );       // �л걤 
	glLightfv( GL_LIGHT0, GL_EMISSION, EmissionColor ); 
    glLightfv( GL_LIGHT0, GL_POSITION, Position );          // ����

    // LIGHT0�� �Ҵ�.
    glEnable( GL_LIGHT0 );
}

void myDisplay(){
	// ��ü�� ���� ǥ��
	GLfloat d1[] = { 0.0,0.5,0.83, 1.0 };
	GLfloat d2[] = { 1.0,1.0,1.0, 1.0 };
	GLfloat d3[] = { 0.7,0.0,0.0, 1.0 };

	glClear (GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	glViewport(0+viewport,0+viewport,600-2*viewport,600-2*viewport); 
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity( );


	// ���콺 �巡�׷� ���� ��ü�� ȸ��
	glRotatef(gYAngle,0.0f,1.0f,0.0f);
	glRotatef(gXAngle,1.0f,0.0f,1.0f);
	glRotatef(gZAngle,0.0f,0.0f,0.0f);
	// gluCylinder �Լ� ����� ���� ���� 
	GLUquadricObj *obj; 
	obj = gluNewQuadric();
	gluQuadricDrawStyle(obj, GLU_FILL);
	gluQuadricNormals(obj, GL_SMOOTH);
	
	// �Ӹ�
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glRotatef(270.0,1.0,0.0,0.0);
	gluSphere(head, 0.3, 50,30);
	glRotatef(90.0,1.0,0.0,0.0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[1]);

	glTranslatef(0.0,-0.43,0.0);
	glRotatef(270.0,1.0,0.0,0.0);
	gluSphere(head, 0.209, 30,20);
	glRotatef(90.0,1.0,0.0,0.0);

	// �����
	glMaterialfv(GL_FRONT,GL_DIFFUSE,d2);
	glTranslatef(0.0,0.15,0.0);
	glRotatef(90,1.0,0.0,0.0);
	glutSolidTorus(0.025,0.15,30,30);

	glRotatef(-90,1.0,0.0,0.0);
	glTranslatef(0.0,-0.17,0.0);
	
	// ���� �ٸ�  
	glMaterialfv(GL_FRONT,GL_DIFFUSE,d2);
	glPushMatrix(); 
	glTranslatef(-0.09,-0.077,0.0);
	glRotatef(75,0.1,0.0,0.0);
	glRotatef(355.0,0.0,1.0,0.0);
	gluCylinder(obj, 0.1, 0.07,0.2,30,10);
	glPopMatrix();

	//// �޹�
	glPushMatrix();
	glTranslatef(-0.1,-0.28,0.06);
	glRotatef(270,0.1,0.0,0.0);
	glRotatef(5.0,0.0,1.0,0.0);
	glutSolidTorus(0.03,0.06,30,30);
	glPopMatrix();

	// ������ �ٸ� 
	glPushMatrix(); 
	glTranslatef(0.09,-0.075,0.0);
	glRotatef(75,0.1,0.0,0.0);
	glRotatef(3,0.0,0.1,0.0);
	gluCylinder(obj, 0.1, 0.07,0.2,30,30); 
	glPopMatrix();

	// ���� ��
	glPushMatrix();
	glTranslatef(0.1,-0.28,0.06);
	glRotatef(90,0.1,0.0,0.0);
	glutSolidTorus(0.03,0.06,30,10);
	glPopMatrix();

	// ������ ��
	glPushMatrix(); 
	glTranslatef(0.13,0.089,0.0);
	glRotatef(90,0.1,0.2,0.0);
	gluCylinder(obj, 0.07, 0.05,0.2,3,3); 

	// ������
	glTranslatef(0.0,-0.007,0.2);
	glutSolidSphere(0.05,20,20);
	glPopMatrix();

	// ���� ��  

	glPushMatrix();
	glTranslatef(-0.13,0.089,0);
	glRotatef(90,0.1,-0.2,0.0);
	gluCylinder(obj, 0.07, 0.05,0.2,3,3);  

	// �޼�
	glTranslatef(0.0,-0.008,0.2);
	glutSolidSphere(0.05,20,20);
	glPopMatrix();

	// ����
	glPushMatrix();
	glRotatef(180,0.0,1.0,0.0);
	glTranslatef(0.0,-0.008,0.2);
	glutSolidSphere(0.05,20,20);
	glPopMatrix();

	glutSwapBuffers();
	gluDeleteQuadric(obj);
}

void myKeyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'z': // zoom in(viewportȮ��, ������ ����)
			viewport+=5;
			glutPostRedisplay();
			break;
		case 'a': // zoom out(viewport ���, ������ ����)
			viewport-=5;
			glutPostRedisplay();
			break;

		case 'q':
			exit(0);
			break;
	}
}

//void myMouseClick(GLint button, GLint state, GLint x, GLint y){
//	if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
//		moveX = x;
//		moveY = y;
//	glutPostRedisplay();
//}

void myMouseMotion(GLint x, GLint y){
	if(x-moveX>0)
		gYAngle += 5.0f;
	else if(x-moveX<0)
		gYAngle -= 5.0f;

	if(y-moveY>0)
		gXAngle += 5.0f;
	else if(y-moveY<0)
		gXAngle -= 5.0f;

	moveX = x;
	moveY = y;
	glutPostRedisplay();
}

AUX_RGBImageRec *loadBMP(char *filename)				
{
	FILE *fp=NULL;									
	if (!filename)										
		return NULL;									

	fp=fopen(filename,"r");						
	if (fp)										
	{
		fclose(fp);								
		return auxDIBImageLoad(filename);				
	}
	return NULL;										
}

int loadGLTextures()									
{
	int Status=FALSE;	
	head = gluNewQuadric();
	gluQuadricDrawStyle(head, GLU_FILL);
	gluQuadricTexture(head, GL_TRUE );
	body = gluNewQuadric();
	gluQuadricDrawStyle(body, GLU_FILL);
	gluQuadricTexture(body, GL_TRUE );
	AUX_RGBImageRec *TextureImage[2];					

	memset(TextureImage,0,sizeof(void *)*1);           

	if (TextureImage[0]=loadBMP("head.bmp"))
	{
		Status=TRUE;								
		glGenTextures(1, &texture[0]);					
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}

	if (TextureImage[1]=loadBMP("body.bmp"))
	{
		Status=TRUE;								
		glGenTextures(1, &texture[1]);					
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[1]->sizeX, TextureImage[1]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[1]->data);
	}

	if (TextureImage[0])								
	{
		if (TextureImage[0]->data)						
			free(TextureImage[0]->data);				
		free(TextureImage[0]);							
	}

	if (TextureImage[1])								
	{
		if (TextureImage[1]->data)						
			free(TextureImage[1]->data);				
		free(TextureImage[1]);							
	}
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	return Status;								
}
