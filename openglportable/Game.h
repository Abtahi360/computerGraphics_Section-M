#ifndef GAME_H
#define GAME_H

#include "iGraphics.h"
#include <windows.h>
#include <mmsystem.h>
#include "Constants.h"
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"
#include "Bullet.h"
#include <vector>
#include <cstdio>
#include <ctime>

enum GameState { LOADING, MENU, NAME_ENTRY, PLAYING, GAME_OVER, CONGRATS, LEADERBOARD };

struct PlayerScore {
    char name[50];
    int score;
};

class Game {
public:
    GameState currentState;
    Player player;
    std::vector<Enemy> enemies;
    Boss boss;
    std::vector<Bullet> bullets;
    std::vector<Bullet> enemyBullets;

    // Game variables
    int currentLevel;
    int currentWave;
    bool isPaused;
    bool showControls;
    bool showCredit;
    bool showHelp;
    bool showMusicSettings;
    bool nameEntered;
    int loadingStartTime;

    // Sound
    DWORD musicVolume;

    // Arrays
    int maxWavesPerLevel[6];
    int levelWaves[5];
    char *levelBackgrounds[5];
    char *enemyImages[5];
    char *bossImages[5];
    int bossMaxHealth[5];
    int bossBulletCount[5];
    int enemySpeed[5];
    int enemyDamage[5];
    int bossDamage[5];

    // Meteors
    int meteorX[2], meteorY[2];

    // Fuel Tank
    bool showFuelTank;
    int fuelTankX, fuelTankY;
    int fuelTickTimer;
    int fuelTankSpawnTimer;
    int timeSinceFuelEmpty;

    // Highscore
    int highScore;
    char highScorer[50];
    PlayerScore topScores[100];
    int totalScores;

    // Menu buttons
    int buttonX, buttonYStart, buttonWidth, buttonHeight, buttonSpacing;
    int hoveredButtonIndex;

    // Music Buttons
    int musicOffBtnX, musicOffBtnY, musicBtnW, musicBtnH;
    int musicOnBtnX, musicOnBtnY;

    Game() {
        currentState = LOADING;
        currentLevel = 1;
        currentWave = 1;
        isPaused = false;
        showControls = false;
        showCredit = false;
        showHelp = false;
        showMusicSettings = false;
        nameEntered = false;
        loadingStartTime = clock();

        musicVolume = 0x50005000;

        int waves[] = {6, 6, 5, 1, 1};
        for(int i=0; i<5; i++) levelWaves[i] = waves[i];

        levelBackgrounds[0] = "background (2).png";
        levelBackgrounds[1] = "background (3).png";
        levelBackgrounds[2] = "background (4).png";
        levelBackgrounds[3] = "bgm(4).png";
        levelBackgrounds[4] = "bgm(4).png";

        enemyImages[0] = "enemy(1).png";
        enemyImages[1] = "enemy(2).png";
        enemyImages[2] = "enemy(3).png";
        enemyImages[3] = "enemy(3).png";
        enemyImages[4] = "enemy(3).png";

        bossImages[0] = "boss(1).png";
        bossImages[1] = "boss(2).png";
        bossImages[2] = "boss(3).png";
        bossImages[3] = "megaboss.png";
        bossImages[4] = "galactus.png";

        int bMaxHealth[] = {50, 80, 120, 200, 500};
        for(int i=0; i<5; i++) bossMaxHealth[i] = bMaxHealth[i];

        int bBulletCount[] = {1, 1, 2, 4, 5};
        for(int i=0; i<5; i++) bossBulletCount[i] = bBulletCount[i];

        int eSpeed[] = {1, 2, 4, 6, 8};
        for(int i=0; i<5; i++) enemySpeed[i] = eSpeed[i];

        int eDamage[] = {30, 90, 270, 300, 1000};
        for(int i=0; i<5; i++) enemyDamage[i] = eDamage[i];

        int bDamage[] = {40, 80, 150, 300, 10000};
        for(int i=0; i<5; i++) bossDamage[i] = bDamage[i];

        buttonX = (SCREEN_WIDTH - buttonWidth) / 3;
        buttonYStart = 555;
        buttonWidth = 500;
        buttonHeight = 60;
        buttonSpacing = 100;
        hoveredButtonIndex = -1;

        musicOffBtnX = 510;
        musicOffBtnY = 105;
        musicBtnW = 145;
        musicBtnH = 95;
        musicOnBtnX = 690;
        musicOnBtnY = 105;

        enemies.resize(5);

        fuelTankX = 100;
        fuelTankY = SCREEN_HEIGHT;
        showFuelTank = false;
        fuelTickTimer = 0;
        fuelTankSpawnTimer = 0;
        timeSinceFuelEmpty = 0;

        highScore = 0;
        strcpy(highScorer, "None");
        totalScores = 0;
    }

    
    void init() {
        spawnEnemies();
        spawnMeteors();
        loadHighScore();
    }

    void applyMusicVolume() {
        waveOutSetVolume(NULL, musicVolume);
    }

    void spawnEnemies() {
        for (int i = 0; i < 5; i++) {
            enemies[i].spawn();
        }
    }

    void spawnMeteors() {
        for (int i = 0; i < 2; i++) {
            meteorX[i] = rand() % (SCREEN_WIDTH - 50);
            meteorY[i] = SCREEN_HEIGHT + rand() % 300;
        }
    }

    void spawnBoss() {
        boss.spawn(currentLevel, bossMaxHealth[currentLevel - 1]);
    }

    void resetGame() {
        player.reset();
        bullets.clear();
        enemyBullets.clear();
        isPaused = false;
        boss.active = false;
        currentWave = 1;
        spawnEnemies();
        spawnMeteors();
    }

    void loadHighScore() {
        FILE *file = fopen("highscore.txt", "r");
        if (file) {
            fscanf(file, "%d %[^\n]", &highScore, highScorer);
            fclose(file);
        }
    }

    void saveHighScore(int s, const char *n) {
        FILE *file = fopen("highscore.txt", "w");
        if (file) {
            fprintf(file, "%d %s", s, n);
            fclose(file);
        }
    }

    void loadLeaderboard() {
        FILE *file = fopen("leaderboard.txt", "r");
        totalScores = 0;

        if (file) {
            while (fscanf(file, "%d %[^\n]", &topScores[totalScores].score, topScores[totalScores].name) == 2) {
                totalScores++;
                if (totalScores == 100) break;
            }
            fclose(file);
        }

        for (int i = 0; i < totalScores - 1; i++) {
            for (int j = i + 1; j < totalScores; j++) {
                if (topScores[j].score > topScores[i].score) {
                    PlayerScore temp = topScores[i];
                    topScores[i] = topScores[j];
                    topScores[j] = temp;
                }
            }
        }
        if (totalScores > 10) totalScores = 10;
    }

    void saveGame() {
        FILE *file = fopen("save.txt", "w");
        if (file) {
            fprintf(file, "%s\n", player.name);
            fprintf(file, "%d %d %d %d %d\n", player.score, player.health, player.fuel, currentLevel, currentWave);
            fclose(file);
        }
    }

    void loadGame() {
        FILE *file = fopen("save.txt", "r");
        if (file) {
            fscanf(file, "%[^\n]\n", player.name);
            fscanf(file, "%d %d %d %d %d", &player.score, &player.health, &player.fuel, &currentLevel, &currentWave);
            fclose(file);

            nameEntered = true;
            currentState = PLAYING;
            boss.active = false;
            bullets.clear();
            enemyBullets.clear();
            isPaused = false;

            spawnEnemies();
            spawnMeteors();

            PlaySound("ingame.wav", NULL, SND_ASYNC | SND_LOOP);
            applyMusicVolume();
        } else {
             // printf("No saved game found.\n");
        }
    }

    void update() {
        if (currentState == LOADING) {
             if ((clock() - loadingStartTime) / CLOCKS_PER_SEC >= 1.5) {
                currentState = MENU;
                PlaySound("menu.wav", NULL, SND_ASYNC | SND_LOOP);
                applyMusicVolume();
             }
             return;
        }
    
        if (currentState != PLAYING || isPaused) return;

        // Update bullets
        for (size_t i = 0; i < bullets.size(); i++) {
            bullets[i].update(20);
            if (bullets[i].y > SCREEN_HEIGHT) {
                bullets.erase(bullets.begin() + i);
                i--;
            }
        }

        // Update enemy bullets
        for (size_t i = 0; i < enemyBullets.size(); i++) {
            enemyBullets[i].update(5 + currentLevel * 2);

            if (enemyBullets[i].y < 0) {
                enemyBullets.erase(enemyBullets.begin() + i);
                i--;
                continue;
            }

            if (enemyBullets[i].x >= player.x && enemyBullets[i].x <= player.x + 100 &&
                enemyBullets[i].y >= player.y && enemyBullets[i].y <= player.y + 100) {

                if (boss.active) {
                    if (currentLevel == 5) player.health = 0;
                    else player.health -= bossDamage[currentLevel - 1];
                } else {
                    player.health -= enemyDamage[currentLevel - 1];
                }

                enemyBullets.erase(enemyBullets.begin() + i);
                i--;
            }
        }

        if (boss.active) {
            boss.update(enemyBullets, currentLevel, bossBulletCount[currentLevel - 1]);

            for (size_t i = 0; i < bullets.size(); i++) {
                if (bullets[i].x >= boss.x && bullets[i].x <= boss.x + 300 &&
                    bullets[i].y >= boss.y && bullets[i].y <= boss.y + 150) {

                    boss.health--;
                    bullets.erase(bullets.begin() + i);
                    i--;

                    if (boss.health <= 0) {
                        boss.active = false;
                        player.score += 100;
                        if (currentLevel < 5) {
                            currentLevel++;
                            currentWave = 1;
                            spawnEnemies();
                            spawnMeteors();
                        } else {
                            currentState = GAME_OVER;
                            PlaySound("missionpass.wav", NULL, SND_ASYNC);
                            applyMusicVolume();
                        }
                    }
                }
            }
        } else {
            for (int i = 0; i < 5; i++) {
                if (enemies[i].active) {
                    enemies[i].update(enemySpeed[currentLevel - 1]);

                    if (!enemies[i].active) {
                        player.health -= enemyDamage[currentLevel - 1];
                    }

                    if (rand() % 60 == 0 && enemyBullets.size() < 50) {
                        enemyBullets.push_back(Bullet(enemies[i].x + 45, enemies[i].y, true));
                    }

                    for (size_t j = 0; j < bullets.size(); j++) {
                        if (bullets[j].x >= enemies[i].x && bullets[j].x <= enemies[i].x + 100 &&
                            bullets[j].y >= enemies[i].y && bullets[j].y <= enemies[i].y + 100) {

                            enemies[i].health -= 3;
                            bullets.erase(bullets.begin() + j);
                            j--;

                            if (enemies[i].health <= 0) {
                                enemies[i].active = false;
                                player.score += 10;
                            }
                        }
                    }
                }
            }

            bool waveCleared = true;
            for(int i=0; i<5; i++) {
                if(enemies[i].active) {
                    waveCleared = false;
                    break;
                }
            }

            if(waveCleared) {
                currentWave++;
                if(currentWave <= levelWaves[currentLevel - 1]) {
                    spawnEnemies();
                } else {
                    spawnBoss();
                }
            }
        }

        for (int i = 0; i < 2; i++) {
            meteorY[i] -= 5;
            if (meteorY[i] < 0) {
                meteorY[i] = SCREEN_HEIGHT + rand() % 300;
                meteorX[i] = rand() % (SCREEN_WIDTH - 50);
            }
            if (player.x + 80 > meteorX[i] && player.x < meteorX[i] + 50 &&
                player.y + 80 > meteorY[i] && player.y < meteorY[i] + 50) {
                player.health -= 10;
                meteorY[i] = SCREEN_HEIGHT + rand() % 300;
                meteorX[i] = rand() % (SCREEN_WIDTH - 50);
            }
        }
        if(showFuelTank) {
            fuelTankY -= 2;
            if(fuelTankY < 0) showFuelTank = false;

            if (player.x + 80 > fuelTankX && player.x < fuelTankX + 50 &&
                player.y + 80 > fuelTankY && player.y < fuelTankY + 50) {
                player.fuel = 100;
                showFuelTank = false;
            }
        }

        fuelTickTimer++;
        if (fuelTickTimer >= 30) {
            player.fuel--;
            fuelTickTimer = 0;
        }

        fuelTankSpawnTimer++;
        if (!showFuelTank && fuelTankSpawnTimer >= 40 * 60) {
            showFuelTank = true;
            fuelTankX = rand() % (SCREEN_WIDTH - 50);
            fuelTankY = SCREEN_HEIGHT;
            fuelTankSpawnTimer = 0;
        }

        if (player.fuel <= 0) {
            timeSinceFuelEmpty++;
            if (timeSinceFuelEmpty >= 5 * 60) {
                player.health -= 10;
                timeSinceFuelEmpty = 0;
            }
        } else {
            timeSinceFuelEmpty = 0;
        }

        if (player.score > highScore) {
            highScore = player.score;
            strcpy(highScorer, player.name);
            saveHighScore(player.score, player.name);
        }

        if (player.health <= 0) {
             FILE *f = fopen("leaderboard.txt", "a");
             if (f) {
                 fprintf(f, "%d %s\n", player.score, player.name);
                 fclose(f);
             }
             if (player.score > highScore) {
                 highScore = player.score;
                 strcpy(highScorer, player.name);
                 saveHighScore(player.score, player.name);
             }
             currentState = GAME_OVER;
             isPaused = false;
             PlaySound("missionpass.wav", NULL, SND_ASYNC);
             applyMusicVolume();
        }
    }

    void draw() {
        iClear();

        if (currentState == LOADING) {
            iShowImage(0, 0, "loadingscreen.png");
            return;
        }

        if (currentState == MENU) {
            iShowImage(0, 0, "menu.png");

            if (hoveredButtonIndex != -1) {
                int expand = 6;
                int bx = buttonX - expand;
                int by = buttonYStart - hoveredButtonIndex * buttonSpacing - expand;
                int bw = buttonWidth + 2 * expand;
                int bh = buttonHeight + 2 * expand;

                iSetColor(0, 255, 250);
                iSetLineWidth(3);
                iRectangle(bx, by, bw, bh);
            }

            if (showMusicSettings) {
                iShowImage(0, 0, "music.png");
                iSetColor(0, 255, 255);
                iText(500, 50, "Press 'h' to get back to the menu page", GLUT_BITMAP_HELVETICA_18);
                return;
            }
            if (showHelp) {
                iShowImage(0, 0, "help.png");
            }
            if (showCredit) {
                 int x = (SCREEN_WIDTH - 800) / 2;
                 int y = (SCREEN_HEIGHT - 600) / 2;
                 iSetColor(255, 255, 255);
                 iFilledRectangle(x - 10, y - 10, 820, 620);
                 iShowImage(x, y, "credit(1).png");
            }
            return;
        }

        if (currentState == NAME_ENTRY) {
            iShowImage(0, 0, "name.png");
            iSetColor(255, 0, 0);
            iText(375, 340, player.name, GLUT_BITMAP_HELVETICA_18);
            return;
        }

        if (currentState == LEADERBOARD) {
            iShowImage(0, 0, "leaderboard_bg.png");
            iSetColor(255, 223, 0);
            iText(SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT - 100, "TOP 10 PLAYERS", GLUT_BITMAP_TIMES_ROMAN_24);

            for (int i = 0; i < totalScores; i++) {
                char entry[100];
                sprintf(entry, "%d. %s - %d", i + 1, topScores[i].name, topScores[i].score);
                iText(SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT - 150 - i * 30, entry, GLUT_BITMAP_HELVETICA_18);
            }
            iText(SCREEN_WIDTH / 2 - 100, 50, "Press 'B' to go back", GLUT_BITMAP_HELVETICA_12);
            return;
        }
        if (currentState == PLAYING || currentState == GAME_OVER) {
            iShowImage(0, 0, levelBackgrounds[currentLevel - 1]);

            player.draw();

            for(auto &b : bullets) b.draw();

            if(boss.active) boss.draw(bossImages[currentLevel - 1]);
            else {
                for(auto &e : enemies) e.draw(enemyImages[currentLevel - 1]);
            }

            for(auto &eb : enemyBullets) eb.draw();

            for(int i=0; i<2; i++) iShowImage(meteorX[i], meteorY[i], "meteor.png");

            if(showFuelTank) iShowImage(fuelTankX, fuelTankY, "fuel.png");

            player.drawHUD();

            char waveStr[50];
            sprintf(waveStr, "Wave: %d%s", currentWave, (boss.active ? " (Boss)" : ""));
            iText(SCREEN_WIDTH - 200, 710, waveStr, GLUT_BITMAP_HELVETICA_12);

            if(showControls) {
                int x = SCREEN_WIDTH - 260;
                int y = 680;
                iSetColor(255, 0, 255);
                iText(x, y, "CONTROLS:", GLUT_BITMAP_HELVETICA_18);
                iText(x, y - 30, "W : Shoot", GLUT_BITMAP_HELVETICA_12);
                iText(x, y - 50, "ESC : Pause / Resume", GLUT_BITMAP_HELVETICA_12);
                iText(x, y - 70, "H : Show/Hide Controls", GLUT_BITMAP_HELVETICA_12);
                iText(x, y - 90, "R : Restart Game", GLUT_BITMAP_HELVETICA_12);
                iText(x, y - 110, "M : Mute Sound", GLUT_BITMAP_HELVETICA_12);
                iText(x, y - 130, "'+' or '-' to increase and decrease the volume", GLUT_BITMAP_HELVETICA_12);
            }

            if(currentState == GAME_OVER) {
                 if (currentLevel == 5 && !boss.active) iShowImage(0, 0, "winscreen.png");
                 else {
                     iShowImage(0, 0, "deathpage.png");
                     char scoreMsg[100];
                     sprintf(scoreMsg, "Your Score: %d", player.score);
                     iSetColor(255, 255, 255);
                     iText(600, 200, scoreMsg, GLUT_BITMAP_TIMES_ROMAN_24);
                 }
                 iText(550, 130, "Press 'L' to return to the Main Menu", GLUT_BITMAP_HELVETICA_18);
            }

            if(isPaused) iShowImage(0, 0, "pause.png");
        }
    }

    void handleInput(unsigned char key) {
        if (currentState == MENU) {
            if (key == 'h' || key == 'H') {
                showHelp = false;
                showCredit = false;
                showMusicSettings = false;
            }
            else if ( (key == 'i' || key == 'I') ) {
                showCredit = !showCredit;
                showHelp = false;
            }
            else if (key == '\r') {
                currentState = NAME_ENTRY;
                PlaySound(0, 0, 0);
            }
            else if (key == 'e' || key == 'E') {
                exit(0);
            }
        }
        else if (currentState == NAME_ENTRY) {
            int len = strlen(player.name);
            if (key == '\r' && len > 0) {
                nameEntered = true;
                currentState = PLAYING;
                PlaySound("ingame.wav", NULL, SND_ASYNC | SND_LOOP);
                applyMusicVolume();
            }
            else if (key == '\b' && len > 0) {
                player.name[len-1] = '\0';
            }
            else if (len < 49 && key != '\r' && key != '\b') {
                player.name[len] = key;
                player.name[len+1] = '\0';
            }
        }
        else if (currentState == PLAYING) {
            if (key == 'w') {
                if (bullets.size() < 50) {
                    bullets.push_back(Bullet(player.x + 57, player.y + 90));
                    PlaySound("fire.wav", NULL, SND_ASYNC | SND_FILENAME | SND_NOSTOP);
                }
            }
            else if (key == 'r') resetGame();
            else if (key == 27) isPaused = !isPaused; // ESC
            else if (key == 'c') showControls = !showControls;
            else if (key == 'm') PlaySound(0, 0, 0);
        }

        if (currentState == GAME_OVER && (key == 'l' || key == 'L')) {
            currentState = MENU;
            resetGame();
            PlaySound("menu.wav", NULL, SND_ASYNC | SND_LOOP);
            applyMusicVolume();
        }

        if (currentState == LEADERBOARD && (key == 'b' || key == 'B')) {
            currentState = MENU;
        }

        // Volume
        if (key == '=' || key == '+') {
            DWORD left = musicVolume & 0xFFFF;
            DWORD right = (musicVolume >> 16) & 0xFFFF;
            left += 0x1000;
            right += 0x1000;
            if (left > 0xFFFF) left = 0xFFFF;
            if (right > 0xFFFF) right = 0xFFFF;
            musicVolume = (right << 16) | left;
            applyMusicVolume();
        }
        else if (key == '-') {
            DWORD left = musicVolume & 0xFFFF;
            DWORD right = (musicVolume >> 16) & 0xFFFF;
            if (left >= 0x1000) left -= 0x1000;
            if (right >= 0x1000) right -= 0x1000;
            musicVolume = (right << 16) | left;
            applyMusicVolume();
        }
    }

    void handleMouseMove(int mx, int my) {
        if (currentState == MENU) {
            hoveredButtonIndex = -1;
            for (int i = 0; i < 6; i++) {
                int bx = buttonX;
                int by = buttonYStart - i * buttonSpacing;
                if (mx >= bx && mx <= bx + buttonWidth &&
                    my >= by && my <= by + buttonHeight) {
                    hoveredButtonIndex = i;
                    break;
                }
            }
        }
        if (currentState == PLAYING && !isPaused) {
            player.setPosition(mx);
        }
    }

    void handleMouseClick(int button, int state, int mx, int my) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            if (currentState == MENU) {
                if (showMusicSettings) {
                    if (mx >= musicOffBtnX && mx <= musicOffBtnX + musicBtnW &&
                        my >= musicOffBtnY && my <= musicOffBtnY + musicBtnH) {
                        PlaySound(0, 0, 0);
                    }
                    if (mx >= musicOnBtnX && mx <= musicOnBtnX + musicBtnW &&
                        my >= musicOnBtnY && my <= musicOnBtnY + musicBtnH) {
                        PlaySound("menu.wav", NULL, SND_ASYNC | SND_LOOP);
                        applyMusicVolume();
                    }
                    return;
                }

                if (hoveredButtonIndex == 0) { // New Game (Enter Name)
                     currentState = NAME_ENTRY;
                     PlaySound(0, 0, 0);
                }
                else if (hoveredButtonIndex == 1) { // Load Game
                     loadGame();
                }
                else if (hoveredButtonIndex == 3) { // Leaderboard
                     currentState = LEADERBOARD;
                     loadLeaderboard();
                }
                else if (hoveredButtonIndex == 5) { // Settings (Music)
                     showMusicSettings = true;
                }
                else if (hoveredButtonIndex == 4) { // Credits
                     showCredit = !showCredit;
                }
                else if (hoveredButtonIndex == 2) { // Exit
                     exit(0);
                }
            }
        }
    }

    void handleSpecialKey(unsigned char key) {
        if (currentState == PLAYING && !isPaused) {
            if (key == GLUT_KEY_RIGHT) player.moveRight();
            if (key == GLUT_KEY_LEFT) player.moveLeft();
        }
    }
};

#endif