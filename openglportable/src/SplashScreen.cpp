#include "SplashScreen.h"
#include <fstream>

using namespace std;

SplashScreen::SplashScreen()
{
    showSplash = true;
    loadBMP("openglportable\images\mera-beta-engineer-banega.bmp");
}

void SplashScreen::loadBMP(const char* filename)
{
    ifstream file(filename, ios::binary);

    if (!file) return;

    unsigned char header[54];
    file.read((char*)header, 54);

    int width  = *(int*)&header[18];
    int height = *(int*)&header[22];
    int imageSize = width * height * 3;

    unsigned char* data = new unsigned char[imageSize];
    file.read((char*)data, imageSize);
    file.close();

    for (int i = 0; i < imageSize; i += 3)
    {
        unsigned char temp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = temp;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, data);

    delete[] data;
}

void SplashScreen::display()
{
    if (!showSplash) return;

    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(0, 0);
        glTexCoord2f(1, 0); glVertex2f(1150, 0);
        glTexCoord2f(1, 1); glVertex2f(1150, 750);
        glTexCoord2f(0, 1); glVertex2f(0, 750);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glFlush();
}
