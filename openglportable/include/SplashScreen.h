#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <GL/glut.h>

class SplashScreen
{
public:
    bool showSplash;
    GLuint textureID;

    SplashScreen();
    void display();
    void loadBMP(const char* filename);
};

#endif
