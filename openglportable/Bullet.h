#ifndef BULLET_H
#define BULLET_H

#include "iGraphics.h"

class Bullet {
public:
    int x, y;
    bool active;
    bool isEnemy; // true if enemy bullet

    Bullet(int startX, int startY, bool enemy = false) {
        x = startX;
        y = startY;
        active = true;
        isEnemy = enemy;
    }

    void update(int speed) {
        if (isEnemy) {
            y -= speed;
        } else {
            y += speed;
        }
    }

    void draw() {
        if (active) {
            if (isEnemy) {
                iSetColor(255, 0, 0);
                iFilledCircle(x, y, 4);
            } else {
                iSetColor(0, 255, 0);
                iFilledRectangle(x, y, 6, 15);
            }
        }
    }
};

#endif
