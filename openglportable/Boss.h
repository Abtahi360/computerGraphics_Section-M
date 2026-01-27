#ifndef BOSS_H
#define BOSS_H

#include "iGraphics.h"
#include "Constants.h"
#include "Bullet.h"
#include <vector>

class Boss {
public:
    int x, y;
    int health;
    int maxHealth;
    bool active;
    int direction; // 1 or -1

    Boss() {
        active = false;
        direction = 1;
    }

    void spawn(int level, int hp) {
        x = SCREEN_WIDTH / 2 - 150;
        y = SCREEN_HEIGHT - 200;
        health = hp;
        maxHealth = hp;
        active = true;
    }

    void update(std::vector<Bullet>& enemyBullets, int currentLevel, int bossBulletCount) {
        if (!active) return;

        x += direction * 4;
        if (x <= 0 || x >= SCREEN_WIDTH - 300) direction = -direction;

        // Shooting logic
        if (rand() % 50 == 0) {
            int bulletsToShoot = (currentLevel >= 2) ? 6 : bossBulletCount;

            for (int b = 0; b < bulletsToShoot; b++) {
                 // Prevent too many bullets
                if (enemyBullets.size() < 200) {
                    int bx = x + 150 + (b * 20) - 60;
                    int by = y;
                    enemyBullets.push_back(Bullet(bx, by, true));
                }
            }
        }

        // Healing logic
        if (currentLevel >= 2 && rand() % 200 == 0) {
            int healAmount = 3 + (currentLevel * 2);
            health += healAmount;
            if (health > maxHealth) health = maxHealth;
        }
    }

    void draw(char* imageName) {
        if (!active) return;

        iShowImage(x, y, imageName);

        // Health bar
        iSetColor(255, 0, 0);
        iFilledRectangle(x, y + 140, health * 5, 20);
        iSetColor(255, 255, 255);
        iText(x, y + 165, "BOSS HEALTH", GLUT_BITMAP_HELVETICA_12);
    }
};

#endif
