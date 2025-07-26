#ifndef TEAM_HEADER_H
#define TEAM_HEADER_H
#include <cstdlib>

#include <string>
#include <glut.h>

// === ENUM FOR GAME LEVEL ===
enum GameLevel { LEVEL1, LEVEL2, GAMEOVER};
enum Direction{LEFT, RIGHT, DOWN, UP};
extern GameLevel currentLevel;
extern float collectibleX, collectibleZ;
extern bool collected;
extern float obstacleX, obstacleZ;
extern const int NUM_ROCKS;
extern const int NUM_SIGNS;
extern const int NUM_COINS;

// === PLAYER STRUCT ===
struct Player {
    float x, y, z;
    float stamina;
    int score;
    float yaw; // rotation around the Y-axis
    bool hasNitro;
	Direction direction; // LEFT or RIGHT
};
struct GameObject {
    float x, y, z;
};
extern GameObject coins[];
extern GameObject rocks[];
extern GameObject signs[];
extern bool coinsCollected[];
extern bool rocksDestroyed[];
extern bool signsDestroyed[];
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
void drawGameOverScreen();

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
