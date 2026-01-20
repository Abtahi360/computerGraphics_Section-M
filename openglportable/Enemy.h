#ifndef ENEMY_H
#define ENEMY_H

#include "iGraphics.h"
#include "Constants.h"

class Enemy {
public:
    int x, y;
    int health; // Represented as 'alive' counter in original code (6)
    bool active;

    Enemy() {
        active = false;
    }

    void spawn() {
        x = rand() % (SCREEN_WIDTH - 100);
        y = SCREEN_HEIGHT - rand() % 200;
        health = 6;
        active = true;
    }

    void update(int speed) {
        if (active) {
            y -= speed;
            if (y < 0) {
                active = false; // Escaped
            }
        }
    }

    void draw(char* imageName) {
        if (active && health > 0) {
            iShowImage(x, y, imageName);
        }
    }
};

#endif
