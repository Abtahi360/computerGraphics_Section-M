/***
 * iGraphics.h: v0.4.0
 * A simple graphics library for C++ using OpenGL and GLUT.
 * Provides functions for drawing shapes, images, and handling input events.
 * This library is designed to be easy to use for beginners and supports basic graphics operations.
 * It includes features like image loading, sprite handling, and collision detection.
 *
 * Author: Mahir Labib Dihan
 * Email: mahirlabibdihan@gmail.com
 * GitHub: https://github.com/mahirlabibdihan
 * Date: July 5, 2025
 */

//
//  Original Author: S. M. Shahriar Nirjon
//  Version: 2.0.2012.2015.2024.2025
//

#pragma once

#include <algorithm>
#include <cmath>
#include <tuple>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#else
// Include POSIX or Linux-specific headers if needed
#include <unistd.h>
#endif

#include "freeglut.h"
#include <time.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>
// #include "glaux.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STBIRDEF extern
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

static int transparent = 1;
static int isFullScreen = 0;
static int isGameMode = 0;
static int programEnded = 0;
const char *iWindowTitle = nullptr;
typedef struct
{
    unsigned char *data;
    int width, height, channels;
    GLuint textureId; // OpenGL texture ID
    // image type svg and non-svg
    bool isSVG; // true if the image is SVG, false if it's a raster image
} Image;

typedef struct
{
    int x, y;
    Image *frames; // Array of individual frame images
    int currentFrame;
    int totalFrames;
    unsigned char *collisionMask;
    // int ignoreColor;

    // Tracking transformation
    float scale;
    bool flipHorizontal, flipVertical;
    float rotation;                         // in radians
    float rotationCenterX, rotationCenterY; // Center of rotation relative to the sprite's top-left corner
} Sprite;

enum MirrorState
{
    NO_MIRROR,
    HORIZONTAL,
    VERTICAL,
    MIRROR_BOTH
};

int iScreenHeight, iScreenWidth;
int iSmallScreenHeight, iSmallScreenWidth;

int iMouseX, iMouseY;
int ifft = 0;

#define MAX_TIMERS 10

void (*iAnimFunction[MAX_TIMERS])(void) = {0};
int iAnimCount = 0;
int iAnimDelays[MAX_TIMERS];
int iAnimPause[MAX_TIMERS];

void iDraw();
void iKeyboard(unsigned char, int);
void iSpecialKeyboard(unsigned char, int);
void iMouseDrag(int, int); // Renamed from iMouseMove to iMouseDrag
void iMouseMove(int, int); // New function
void iMouse(int button, int state, int x, int y);
void iMouseWheel(int dir, int x, int y);
// void iResize(int width, int height);

#define mmax(a, b) ((a) > (b) ? (a) : (b))
#define mmin(a, b) ((a) < (b) ? (a) : (b))
#define sswap(a, b)           \
    do                        \
    {                         \
        typeof(a) temp = (a); \
        (a) = (b);            \
        (b) = temp;           \
    } while (0)

#ifdef WIN32

#ifdef __x86_64
#define IMSEC unsigned long long
#else
#define IMSEC unsigned int
#endif

#endif

void timerCallback(int index)
{
    if (!iAnimPause[index] && iAnimFunction[index])
    {
        // int currentTime = glutGet(GLUT_ELAPSED_TIME);             // milliseconds since start
        // int deltaTime = (currentTime - iAnimLastCallTime[index]); // in seconds
        // iAnimLastCallTime[index] = currentTime;
        iAnimFunction[index]();
    }

    glutTimerFunc(iAnimDelays[index], timerCallback, index);
}

int iSetTimer(int msec, void (*f)(void))
{
    if (iAnimCount >= 10)
    {
        printf("Error: Maximum number of timers reached.\n");
        return -1;
    }

    int index = iAnimCount++;
    iAnimFunction[index] = f;
    iAnimDelays[index] = msec;
    iAnimPause[index] = 0;

    glutTimerFunc(msec, timerCallback, index);
    return index;
}

void iPauseTimer(int index)
{
    if (index >= 0 && index < iAnimCount)
    {
        iAnimPause[index] = 1;
    }
}

void iResumeTimer(int index)
{
    if (index >= 0 && index < iAnimCount)
    {
        iAnimPause[index] = 0;
    }
}

void iUpdateTexture(Image *img, bool resized = false)
{
    if (!img->textureId)
    {
        return; // No texture to update
    }
    glBindTexture(GL_TEXTURE_2D, img->textureId);
    GLenum format = (img->channels == 4) ? GL_RGBA : GL_RGB;

    if (resized)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format,
                     img->width, img->height, 0,
                     format, GL_UNSIGNED_BYTE, img->data);
    }
    else
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        img->width, img->height,
                        format, GL_UNSIGNED_BYTE, img->data);
    }
}

void iIgnorePixels(Image *img, int ignoreColor = -1)
{
    if (ignoreColor == -1 || img->data == nullptr)
        return;

    unsigned char ignoreR = (ignoreColor >> 16) & 0xFF;
    unsigned char ignoreG = (ignoreColor >> 8) & 0xFF;
    unsigned char ignoreB = ignoreColor & 0xFF;

    for (int i = 0; i < img->width * img->height * img->channels; i += img->channels)
    {
        if (img->data[i] == ignoreR && img->data[i + 1] == ignoreG && img->data[i + 2] == ignoreB)
        {
            if (img->channels == 4)
            {
                img->data[i + 3] = 0; // Set alpha to 0 for transparency
            }
            else
            {
                img->data[i] = img->data[i + 1] = img->data[i + 2] = 0; // Set RGB to black
            }
        }
    }

    iUpdateTexture(img); // Update the OpenGL texture after modifying pixel data
}

bool iLoadTexture(Image *img)
{
    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    // Set texture parameters ONCE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Determine format
    GLenum format = (img->channels == 4) ? GL_RGBA : GL_RGB;

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, img->width, img->height,
                 0, format, GL_UNSIGNED_BYTE, img->data);

    img->textureId = texId;
    return true;
}

bool iLoadSVG(Image *img, const char *filepath, double scale = 1.0)
{
    // Load SVG
    NSVGimage *image = nsvgParseFromFile(filepath, "px", 96.0f);
    if (!image)
    {
        fprintf(stderr, "Could not open SVG file: %s\n", filepath);
        return false;
    }

    int origW = (int)image->width;
    int origH = (int)image->height;

    int outW = (int)(origW * scale);
    int outH = (int)(origH * scale);

    // printf("SVG image size: %d x %d, scaled to: %d x %d\n", origW, origH, outW, outH);

    img->data = (unsigned char *)malloc(outW * outH * 4);
    if (!img->data)
    {
        fprintf(stderr, "Failed to allocate image buffer\n");
        nsvgDelete(image);
        return false;
    }

    NSVGrasterizer *rast = nsvgCreateRasterizer();
    if (!rast)
    {
        fprintf(stderr, "Failed to create rasterizer\n");
        free(img->data);
        nsvgDelete(image);
        return false;
    }

    nsvgRasterize(rast, image, 0, 0, scale, img->data, outW, outH, outW * 4);

    img->width = outW;
    img->height = outH;
    img->channels = 4; // RGBA
    img->isSVG = true; // Mark as SVG image
    img->textureId = 0;

    nsvgDeleteRasterizer(rast);
    nsvgDelete(image);

    return true;
}

// Additional functions for displaying images
bool iLoadImage2(Image *img, const char filename[], int ignoreColor = -1)
{
    // Check if the image is svg based on extension
    const char *ext = strrchr(filename, '.');

    stbi_set_flip_vertically_on_load(true);
    if (ext && (strcmp(ext, ".svg") == 0 || strcmp(ext, ".SVG") == 0))
    {
        iLoadSVG(img, filename);
    }
    else
    {
        img->data = stbi_load(filename, &img->width, &img->height, &img->channels, 0);
        img->isSVG = false; // Mark as non-SVG image
    }

    if (img->data == nullptr)
    {
        printf("ERROR: Failed to load image: %s\n", stbi_failure_reason());
        return false;
    }

    // Ignore the pixels with the specified ignore color
    iIgnorePixels(img, ignoreColor);
    img->textureId = 0; // Initialize texture ID to 0
    return true;
}

bool iLoadImage(Image *img, const char filename[])
{
    iLoadImage2(img, filename, -1);
}
