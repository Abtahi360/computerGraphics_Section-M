#include <GL/glut.h>
#include "CoverPage.h"

CoverPage coverPage;

void display()
{
    coverPage.display();
}
void init()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1150, 0, 750);
}
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1150, 750);
    glutInitWindowPosition(80, 50);
    glutCreateWindow("Cover Page");

    init();
    glutDisplayFunc(display);

    glutMainLoop();
    return 0;
}
