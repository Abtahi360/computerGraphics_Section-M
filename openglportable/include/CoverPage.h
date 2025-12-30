#ifndef COVERPAGE_H
#define COVERPAGE_H
#include <GL/glut.h>

class CoverPage
{
public:
    void display();

private:
    void renderBitmapString(float x, float y, void* font, const char* string);
    void renderCenteredString(float x, float y, void* font, const char* string);
    void horizontal_Line(float x1, float y1, float x2, float y2);
    void verticle_Line(float x1, float y1, float x2, float y2);
};

#endif
