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

void iFreeTexture(Image *img)
{
    if (!img || img->textureId == 0)
        return;
    glDeleteTextures(1, &img->textureId);
    img->textureId = 0; // Reset texture ID after deletion
}

void iFreeImage(Image *img)
{
    iFreeTexture(img);
    stbi_image_free(img->data);
}

void iLine(double x1, double y1, double x2, double y2)
{
    glBegin(GL_LINE_STRIP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void iRectangle(double left, double bottom, double dx, double dy)
{
    double x1, y1, x2, y2;

    x1 = left;
    y1 = bottom;
    x2 = x1 + dx;
    y2 = y1 + dy;

    iLine(x1, y1, x2, y1);
    iLine(x2, y1, x2, y2);
    iLine(x2, y2, x1, y2);
    iLine(x1, y2, x1, y1);
}

void iShowTexture2(int x, int y, Image *img, int width = -1, int height = -1, MirrorState mirror = NO_MIRROR)
{
    int imgWidth = width == -1 ? img->width : width;
    int imgHeight = height == -1 ? img->height : height;

    if (x + imgWidth <= 0 || y + imgHeight <= 0 || x >= iScreenWidth || y >= iScreenHeight)
        return;
    if (img->textureId == 0)
    {
        if (!iLoadTexture(img))
        {
            printf("Failed to load texture for image at (%d, %d)\n", x, y);
            return;
        }
    }

    // iRectangle(x, y, imgWidth, imgHeight); // Uncomment for debugging rectangle bounds

    glBindTexture(GL_TEXTURE_2D, img->textureId);

    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);

    float tx1 = 0.0f, ty1 = 0.0f;
    float tx2 = 1.0f, ty2 = 1.0f;

    // Handle mirror states
    if (mirror == HORIZONTAL || mirror == MIRROR_BOTH)
        sswap(tx1, tx2);
    if (mirror == VERTICAL || mirror == MIRROR_BOTH)
        sswap(ty1, ty2);

    if (img->isSVG) // If the image is an SVG, we need to flip vertically
    {
        // SVG images are typically flipped vertically in OpenGL
        sswap(ty1, ty2);
    }
    glTexCoord2f(tx1, ty1);
    glVertex2i(x, y);
    glTexCoord2f(tx2, ty1);
    glVertex2i(x + imgWidth, y);
    glTexCoord2f(tx2, ty2);
    glVertex2i(x + imgWidth, y + imgHeight);
    glTexCoord2f(tx1, ty2);
    glVertex2i(x, y + imgHeight);

    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void iShowLoadedImage2(int x, int y, Image *img, int width = -1, int height = -1, MirrorState mirror = NO_MIRROR)
{
    iShowTexture2(x, y, img, width, height, mirror);
}

void iShowLoadedImage(int x, int y, Image *img)
{
    iShowLoadedImage2(x, y, img);
}

void iShowImage2(int x, int y, const char *filename, int ignoreColor = -1)
{
    Image img;
    if (!iLoadImage2(&img, filename, ignoreColor))
    {
        printf("ERROR: Failed to load image: %s\n", filename);
        return;
    }
    iShowTexture2(x, y, &img, -1, -1, NO_MIRROR);
    iFreeImage(&img);
}

void iShowImage(int x, int y, const char *filename)
{
    iShowImage2(x, y, filename);
}

void iShowSVG2(double x, double y, const char *filepath, double scale = 1.0, MirrorState mirror = NO_MIRROR)
{
    // Load SVG
    Image img;
    if (!iLoadSVG(&img, filepath, scale))
    {
        printf("ERROR: Failed to load svg: %s\n", filepath);
        return;
    }
    iShowTexture2(x, y, &img, img.width, img.height, mirror);
    iFreeImage(&img);
}

void iShowSVG(double x, double y, const char *filepath)
{
    iShowSVG2(x, y, filepath);
}

void iShowLoadedSVG2(double x, double y, Image *img, MirrorState mirror = NO_MIRROR)
{
    // Ensure the image is an SVG
    if (!img->isSVG)
    {
        fprintf(stderr, "Image is not an SVG.\n");
        return;
    }

    // Load the SVG texture if not already loaded
    if (img->textureId == 0)
    {
        iLoadTexture(img);
    }

    iShowTexture2(x, y, img, img->width, img->height, mirror);
}

void iShowLoadedSVG(double x, double y, Image *img)
{
    iShowLoadedSVG2(x, y, img);
}

void iWrapImage(Image *img, int dx = 0, int dy = 0)
{
    // Circular shift the image horizontally by dx and vertically by dy pixels
    int width = img->width;
    int height = img->height;
    int channels = img->channels;
    unsigned char *data = img->data;
    unsigned char *wrappedData = new unsigned char[width * height * channels];

    // Normalize dx to [0, width), dy to [0, height)
    dx = ((dx % width) + width) % width;
    dy = ((dy % height) + height) % height;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int srcIndex = (y * width + x) * channels;
            int wrappedX = (x + dx) % width;
            int wrappedY = (y + dy) % height;
            int dstIndex = (wrappedY * width + wrappedX) * channels;

            for (int c = 0; c < channels; c++)
            {
                wrappedData[dstIndex + c] = data[srcIndex + c];
            }
        }
    }

    stbi_image_free(data);
    img->data = wrappedData;

    iUpdateTexture(img);
}

void iResizeImage(Image *img, int width, int height)
{
    int imgWidth = img->width;
    int imgHeight = img->height;
    int channels = img->channels;
    unsigned char *data = img->data;
    unsigned char *resizedData = new unsigned char[width * height * channels];
    stbir_pixel_layout layout;
    if (channels == 3)
        layout = STBIR_RGB;
    else if (channels == 4)
        layout = STBIR_RGBA;
    else
    {
        // handle error
    }
    stbir_resize_uint8_srgb(data, imgWidth, imgHeight, 0, resizedData, width, height, 0, layout);
    // stbir_resize_uint8(data, imgWidth, imgHeight, 0, resizedData, width, height, 0, channels);
    stbi_image_free(data);
    img->data = resizedData;
    img->width = width;
    img->height = height;

    iUpdateTexture(img, true); // Update OpenGL texture after resizing
}

void iScaleImage(Image *img, double scale)
{
    if (!img || scale <= 0.0f)
        return;

    int newWidth = (int)(img->width * scale);
    int newHeight = (int)(img->height * scale);

    int channels = img->channels;
    unsigned char *data = img->data;
    unsigned char *resizedData = new unsigned char[newWidth * newHeight * channels];

    stbir_pixel_layout layout;
    if (channels == 3)
        layout = STBIR_RGB;
    else if (channels == 4)
        layout = STBIR_RGBA;
    else
    {
        // handle error
    }
    stbir_resize_uint8_srgb(data, img->width, img->height, 0, resizedData, newWidth, newHeight, 0, layout);
    // stbir_resize_uint8(
    //     data, img->width, img->height, 0,
    //     resizedData, newWidth, newHeight, 0,
    //     channels);

    stbi_image_free(data);
    img->data = resizedData;
    img->width = newWidth;
    img->height = newHeight;

    iUpdateTexture(img, true); // Update OpenGL texture after scaling
}

void iMirrorImage(Image *img, MirrorState state)
{
    int width = img->width;
    int height = img->height;
    int channels = img->channels;
    unsigned char *data = img->data;
    unsigned char *mirroredData = new unsigned char[width * height * channels];
    if (state == HORIZONTAL)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int index = (y * width + x) * channels;
                int mirroredIndex = (y * width + (width - x - 1)) * channels;
                for (int c = 0; c < channels; c++)
                {
                    mirroredData[mirroredIndex + c] = data[index + c];
                }
            }
        }
    }
    else if (state == VERTICAL)
    {
        for (int y = 0; y < height; y++)
        {
            int mirroredY = height - y - 1;
            for (int x = 0; x < width; x++)
            {
                int index = (y * width + x) * channels;
                int mirroredIndex = (mirroredY * width + x) * channels;
                for (int c = 0; c < channels; c++)
                {
                    mirroredData[mirroredIndex + c] = data[index + c];
                }
            }
        }
    }
    stbi_image_free(data);
    img->data = mirroredData;

    iUpdateTexture(img); // Update OpenGL texture after mirroring
}

void iUpdateCollisionMask(Sprite *s)
{
    if (!s || !s->frames)
    {
        return;
    }
    Image *frame = &s->frames[s->currentFrame];
    int width = frame->width;
    int height = frame->height;
    int channels = frame->channels;
    unsigned char *data = frame->data;

    if (s->collisionMask != nullptr)
    {
        delete[] s->collisionMask;
    }

    unsigned char *collisionMask = new unsigned char[width * height];

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = (y * width + x) * channels;
            unsigned char a = (channels == 4) ? data[index + 3] : 255;
            bool isTransparent = (channels == 4 && a == 0);
            collisionMask[y * width + x] = (isTransparent) ? 0 : 1;
        }
    }
    s->collisionMask = collisionMask;
}

int iCheckImageSpriteCollision(int x1, int y1, Image *img, Sprite *s)
{
    if (!img || !s || !s->frames || s->currentFrame < 0 || s->currentFrame >= s->totalFrames)
        return 0; // Invalid image or sprite

    Image *frame = &s->frames[s->currentFrame];
    int x2 = s->x;
    int y2 = s->y;

    // Calculate bounding box overlap
    int overlapMinX = mmax(x1, x2);
    int overlapMaxX = mmin(x1 + img->width, x2 + frame->width);
    int overlapMinY = mmax(y1, y2);
    int overlapMaxY = mmin(y1 + img->height, y2 + frame->height);

    if (overlapMinX >= overlapMaxX || overlapMinY >= overlapMaxY)
        return 0; // No overlap

    int count = 0;
    // Check pixel-perfect collision in the overlapping area
    for (int y = overlapMinY; y < overlapMaxY; y++)
    {
        for (int x = overlapMinX; x < overlapMaxX; x++)
        {
            // Get pixel coordinates in both images
            int localX1 = x - x1;
            int localY1 = y - y1;
            int localX2 = x - x2;
            int localY2 = y - y2;

            if (localX1 < 0 || localY1 < 0 || localX1 >= img->width || localY1 >= img->height ||
                localX2 < 0 || localY2 < 0 || localX2 >= frame->width || localY2 >= frame->height)
                continue;

            unsigned char *pixel1 = &img->data[(localY1 * img->width + localX1) * img->channels];
            unsigned char *pixel2 = &frame->data[(localY2 * frame->width + localX2) * frame->channels];

            // Check if both pixels are not transparent
            bool isPixel1Transparent = (img->channels == 4 && pixel1[3] == 0);
            bool isPixel2Transparent = (frame->channels == 4 && pixel2[3] == 0);

            if (!isPixel1Transparent && !isPixel2Transparent)
            {
                // Both pixels are opaque, collision detected
                count++;
            }
        }
    }
    return count;
}

