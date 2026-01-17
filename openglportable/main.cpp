#include <GL/glut.h>
#include "CoverPage.h"
#include "SplashScreen.h"

CoverPage coverPage;
SplashScreen splash;

void display()
{
    if (splash.showSplash)
        splash.display();
    else
        coverPage.display();
}
void timer(int value)
{
    splash.showSplash = false;   // splash off
    glutPostRedisplay();
}
void init()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1150, 0, 750);
    glClearColor(0, 0, 0, 1);
}
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1150, 750);
    glutInitWindowPosition(80, 50);
    glutCreateWindow("Project");

    init();
    glutDisplayFunc(display);
    glutTimerFunc(5000, timer, 0);
    glutMainLoop();
    return 0;
}
