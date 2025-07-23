#ifndef TEAM_HEADER_H
#define TEAM_HEADER_H
#include <cstdlib>

#include <string>
#include <glut.h>

// === ENUM FOR GAME LEVEL ===
enum GameLevel { LEVEL1, LEVEL2 };
extern GameLevel currentLevel;

// === PLAYER STRUCT ===
struct Player {
    float x, y, z;
    float stamina;
    int score;
    bool hasNitro;
};

// === GLOBAL VARIABLES ===
extern Player player;
extern float moveSpeed;
extern bool isFirstPerson;

// === TEAM 1 - CORE SYSTEMS ===
void handleKeyboard(unsigned char key, int x, int y);
void handleMouse(int button, int state, int x, int y);
void toggleCameraView();
void updateCamera();
void drawHUD();
void updateLevel1Logic();
void updateLevel2Logic();
void updateLighting();
void playSound(const std::string& type);
void initGame();

// === TEAM 2 - LEVEL 1 ===
void initLevel1();
void drawLevel1();
void animateLevel1Objects();
void initSounds(); // shared from team 2

// === TEAM 3 - LEVEL 2 ===
void initLevel2();
void drawLevel2();
void animateLevel2Objects();
void initLighting(); // shared from team 3

#endif
