#ifndef PLAYER_H
#define PLAYER_H

#include "iGraphics.h"
#include "Constants.h"
#include <string>
#include <cstring>

class Player {
public:
    int x, y;
    int health;
    int fuel;
    int score;
    char name[50];

    Player() {
        reset();
        strcpy(name, "");
    }

    void reset() {
        x = SCREEN_WIDTH / 2;
        y = 50;
        health = 100;
        fuel = 100;
        score = 0;
    }

    void moveLeft() {
        x -= 20;
        if (x < 0) x = 0;
    }

    void moveRight() {
        x += 20;
        if (x > SCREEN_WIDTH - 100) x = SCREEN_WIDTH - 100;
    }

    void setPosition(int mx) {
        x = mx - 50;
        if (x < 0) x = 0;
        if (x > SCREEN_WIDTH - 100) x = SCREEN_WIDTH - 100;
    }

    void draw() {
        iShowImage(x, y, "ship(1).png");
    }

    void drawHUD() {
        // Health bar
        iSetColor(255, 0, 0);
        iFilledRectangle(20, 720, health * 2, 20);
        iSetColor(255, 255, 255);
        iSetColor(255, 0, 0);
        iText(20, 745, "Health", GLUT_BITMAP_HELVETICA_12);

        // Fuel bar
        iSetColor(0, 255, 0);
        iFilledRectangle(20, 690, fuel, 20);
        iSetColor(255, 255, 255);
        iSetColor(0, 255, 0);
        iText(20, 675, "Fuel", GLUT_BITMAP_HELVETICA_12);

        std::string scoreStr = "Score: " + std::to_string(score);

        iText(SCREEN_WIDTH - 200, 740, (char*)scoreStr.c_str(), GLUT_BITMAP_HELVETICA_12);
    }
};

#endif
