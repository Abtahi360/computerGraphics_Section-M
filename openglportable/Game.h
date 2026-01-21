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
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>

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
        loadingStartTime = static_cast<int>(clock());

        musicVolume = 0x50005000;

        int waves[] = {6, 6, 5, 1, 1};
        for(int i=0; i<5; i++) levelWaves[i] = waves[i];

        levelBackgrounds[0] = (char*)"background (2).png";
        levelBackgrounds[1] = (char*)"background (3).png";
        levelBackgrounds[2] = (char*)"background (4).png";
        levelBackgrounds[3] = (char*)"bgm(4).png";
        levelBackgrounds[4] = (char*)"bgm(4).png";

        enemyImages[0] = (char*)"enemy(1).png";
        enemyImages[1] = (char*)"enemy(2).png";
        enemyImages[2] = (char*)"enemy(3).png";
        enemyImages[3] = (char*)"enemy(3).png";
        enemyImages[4] = (char*)"enemy(3).png";

        bossImages[0] = (char*)"boss(1).png";
        bossImages[1] = (char*)"boss(2).png";
        bossImages[2] = (char*)"boss(3).png";
        bossImages[3] = (char*)"megaboss.png";
        bossImages[4] = (char*)"galactus.png";

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

        buttonWidth = 500; // ensure width defined before using in calculation
        buttonX = (SCREEN_WIDTH - buttonWidth) / 3;
        buttonYStart = 555;
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
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
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
            meteorX[i] = std::rand() % (SCREEN_WIDTH - 50);
            meteorY[i] = SCREEN_HEIGHT + std::rand() % 300;
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
        std::ifstream infile("highscore.txt");
        if (infile.is_open()) {
            int score = 0;
            if (infile >> score) {
                highScore = score;
                std::string rest;
                std::getline(infile, rest); // read remainder of the line (name)
                // trim leading spaces
                if (!rest.empty() && rest[0] == ' ') rest.erase(0, 1);
                if (!rest.empty()) {
                    std::strncpy(highScorer, rest.c_str(), sizeof(highScorer)-1);
                    highScorer[sizeof(highScorer)-1] = '