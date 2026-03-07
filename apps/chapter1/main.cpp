#include "entry.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("example");
	glutReshapeFunc(SJH::Chapter1::reshape);
	glutDisplayFunc(SJH::Chapter1::display);
	glutMainLoop();
}
