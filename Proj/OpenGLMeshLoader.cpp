#define GLUT_DISABLE_ATEXIT_HACK // Disable glut's atexit registration
#include <Windows.h>
#include "teamHeader.h"
#include <glut.h> 
#include <string> 
#include <cmath>       
#include <algorithm> 
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#undef exit
using namespace std;

// === GLOBAL DEFINITIONS ===
Player player = { 0.0f, 0.0f, 0.0f, 0.0f, 0, false };
float moveSpeed = 0.5f;
bool isFirstPerson = false;
GameLevel currentLevel = LEVEL1;
float sunIntensity = 0.5f;
bool carSoundPlaying = false;
//fake collectibles
float foodX = 10.0f;
float foodZ = -5.0f;
float drinkX = 12.0f;
float drinkZ = -8.0f;
bool foodCollected = false;
bool drinkCollected = false;
float collectibleX = 10.0f;
float collectibleZ = -5.0f;
bool collected = false;
float obstacleX = 15.0f;
float obstacleZ = -10.0f;

// === INIT ===
void initGame() {
    glEnable(GL_DEPTH_TEST);
    initLevel1();     // Team 2
    initLevel2();     // Team 3
    initSounds();     // Team 2
    initLighting();   // Team 3
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);//allows gl color to affect material
}

// === INPUT ===
void handleKeyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w': case 'W':
    case 's': case 'S':
    case 'a': case 'A':
    case 'd': case 'D':
        // Movement logic
        if (key == 'w' || key == 'W') {
            player.z -= moveSpeed;
            player.playerAngle = 0.0f;
        }
        if (key == 's' || key == 'S') {
            player.z += moveSpeed;
        }
        if (key == 'a' || key == 'A') {
            player.x -= moveSpeed;
            player.playerAngle = 90.0f;
        }
        if (key == 'd' || key == 'D') {
            player.x += moveSpeed;
            player.playerAngle = -90.0f;
        }

        // Start car movement sound (once)
        if (!carSoundPlaying) {
            playSound("car_move");
            carSoundPlaying = true;
        }
        break;

    case 'l': case 'L':
        currentLevel = LEVEL2;
        player.stamina = 100;
        player.z = 300.0f;
        break;

    case 27:  ::exit(EXIT_SUCCESS); break; // ESC
    }
}


void handleMouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        toggleCameraView();
    }
}

void toggleCameraView() {
    isFirstPerson = !isFirstPerson;
}

// === CAMERA ===
void updateCamera() {
    if (isFirstPerson) {
        gluLookAt(player.x, player.y + 1.0f, player.z,
            player.x, player.y + 1.0f, player.z - 5,
            0.0f, 1.0f, 0.0f);
    }
    else {
        gluLookAt(player.x, player.y + 5.0f, player.z + 10.0f,
            player.x, player.y, player.z,
            0.0f, 1.0f, 0.0f);
    }
}

// === HUD ===
void drawHUD() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glRasterPos2i(10, 580);
    std::string hud = "Score: " + std::to_string(player.score) + " | Stamina: " + std::to_string((int)player.stamina);
    for (char c : hud) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / (float)h, 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

// === LOGIC ===
void updateLevel1Logic() {
    // === FOOD COLLECTION === 
    if (!foodCollected) {
        float dx = player.x - foodX;
        float dz = player.z - foodZ;
        float dist = sqrt(dx * dx + dz * dz);
        if (dist < 2.0f) {
            player.stamina = min(player.stamina + 10, 100.0f);
            playSound("chew");
            foodCollected = true;
        }
    }

    // === DRINK COLLECTION ===
    if (!drinkCollected) {
        float dx = player.x - drinkX;
        float dz = player.z - drinkZ;
        float dist = sqrt(dx * dx + dz * dz);
        if (dist < 2.0f) {
            player.stamina = min(player.stamina + 15, 100.0f);
            playSound("drink");
            drinkCollected = true;
        }
    }

    float dx2 = player.x - obstacleX;
    float dz2 = player.z - obstacleZ;
    float dist2 = sqrt(dx2 * dx2 + dz2 * dz2);
    if (dist2 < 2.0f) {
        player.stamina -= 1.0f;
        playSound("car_crash");
        if (player.stamina <= 0) {
            player.x = 0;
            player.z = 0;
            player.stamina = 100;
        }
    }



    // === LEVEL TRANSITION ===
    if (player.x > 30.0f) {
        currentLevel = LEVEL2;
        player.x = 0;
        player.z = 0;
        playSound("levelup");  // Optional: add level transition sound
    }
}



void updateLevel2Logic() {
    // --- 1. Continuous Stamina Decrease ---
    // Requirement: "The player has to move through the dessert and tries to survive till he reaches the target."
    // This implies a constant challenge, so we'll decrease stamina over time.
    player.stamina -= 0.03f; // Decrease stamina by a small amount each frame. Adjust this value to make it faster/slower.

    // --- 2. Check for Game Over Condition ---
    if (player.stamina <= 0) {
        currentLevel = GAMEOVER; // Change the game state.
        return; // Stop processing the rest of the level logic for this frame.
    }

    // --- 3. Coin Collection Logic ---
    for (int i = 0; i < NUM_COINS; i++) {
        // Check the new array
        if (!coinsCollected[i]) {
            float dx = player.x - coins[i].x;
            float dz = player.z - coins[i].z;
            if (sqrt(dx * dx + dz * dz) < 2.0f) {
                player.score += 10;
                coinsCollected[i] = true; // Update the new array
                playSound("collect_coin");
            }
        }
    }

    // --- 4. Obstacle Collision Logic ---
    // Colliding with obstacles will now cause a larger, immediate drop in stamina.
      // --- Check for collision with ALL rocks ---
    for (int i = 0; i < NUM_ROCKS; i++) {
        float dx = player.x - rocks[i].x;
        float dz = player.z - rocks[i].z;
        float distance = sqrt(dx * dx + dz * dz);
        float collisionRadius = 2.0f; // The "size" of the player and rock combined

        if (distance < collisionRadius) {
            // A collision has occurred.
            player.stamina -= 0.1f; // A very small stamina penalty for the bump.
            playSound("car_crash");

            // --- Solid Collision Response ---
            // Calculate how much the player has penetrated the obstacle.
            float overlap = collisionRadius - distance;

            // Push the player back along the vector from the obstacle's center
            // to the player's center, just enough to resolve the overlap.
            player.x += (dx / distance) * overlap;
            player.z += (dz / distance) * overlap;
        }
    }


    // --- Check for collision with ALL signs ---
    for (int i = 0; i < NUM_SIGNS; i++) {
        // First, check if this sign has already been destroyed.
        if (!signsDestroyed[i]) {
            float dx = player.x - signs[i].x;
            float dz = player.z - signs[i].z;
            if (sqrt(dx * dx + dz * dz) < 2.0f) {
                // FIX: Reduce the stamina penalty.
                player.stamina -= 5.0f;
                playSound("car_crash");

                // NEW: Mark this sign as destroyed.
                signsDestroyed[i] = true;
            }
        }
    }

    // --- 5. Win Condition ---
    if (player.z < -49.0f) {
        playSound("cheer"); // Play cheering sound
        MessageBoxA(NULL, "You reached the finish line!", "Level 2 Complete!", MB_OK);
        ::exit(EXIT_SUCCESS);
    }
}
// Add this new function anywhere in your main OpenGLMeshLoader.cpp file.
void drawGameOverScreen() {
    // Switch to 2D orthographic projection to draw text.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 600, 0); // Using 800x600 window size. Top-left is (0,0).

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Disable 3D features like lighting and depth testing.
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    // Draw a semi-transparent black background to dim the scene.
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(800, 0);
    glVertex2i(800, 600);
    glVertex2i(0, 600);
    glEnd();

    // Draw the "GAME OVER" text in the center of the screen.
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for the text.
    glRasterPos2i(350, 300); // Position the text.
    const char* message = "GAME OVER";
    for (const char* c = message; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }

    // === Draw "Final Score" Text ===
    std::string scoreMsg = "Final Score: " + std::to_string(player.score);
    glRasterPos2i(330, 340); // Positioned above the GAME OVER text
    for (char c : scoreMsg) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // Re-enable features before exiting the function.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    // Restore the previous matrices.
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}



// === LIGHTING ===
void updateLighting() {
    //sunlight
    glEnable(GL_LIGHT0);

    //simulate sunlight intensity over time(sine wave)
    float sunIntensity = 0.5f + 0.5f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.001f);
    GLfloat diffuseSun[] = { sunIntensity,sunIntensity,sunIntensity,1.0f };
    GLfloat sunDir[] = { -0.3f,-1.0f,-0.5f,0.0f };//directional sunlight
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseSun);
    glLightfv(GL_LIGHT0, GL_POSITION, sunDir);
    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);


    glEnable(GL_LIGHT1);

    GLfloat light_pos[] = { player.x, player.y + 0.5f, player.z, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, light_pos);

    GLfloat light_dir[] = { 0.0f, -0.5f, -10.0f };
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_dir);

    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 25.0f);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 50.0f);;
}




// === SOUND ===
void playSound(const std::string& type) {
    if (type == "chew") {
        PlaySound(TEXT("chew.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
    else if (type == "drink") {
        PlaySound(TEXT("drink.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
    else if (type == "collect_coin") {
        PlaySound(TEXT("coin_collect.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
    else if (type == "car_crash") {
        PlaySound(TEXT("car_crash.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
    else if (type == "car_move") {
        PlaySound(TEXT("car_move.wav"), NULL, SND_LOOP | SND_ASYNC);
    }
    else if (type == "levelup") {
        PlaySound(TEXT("level_up.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
    else if (type == "win") {
        PlaySound(TEXT("cheer.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
}

// === DISPLAY ===
void display() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    updateLighting();
    updateCamera();
    if (currentLevel == LEVEL1) drawLevel1();
    else {
        drawLevel2();
        animateLevel2Objects();
    }
    if (currentLevel == GAMEOVER) {
        drawGameOverScreen();
    }

    drawHUD();
    glutSwapBuffers();
}

// === TIMER ===
void timer(int value) {
    if (currentLevel != GAMEOVER) {
        if (currentLevel == LEVEL1) {
            updateLevel1Logic();     // T1 - logic
            animateLevel1Objects();  // T2 - animation
        }
        else {
            updateLevel2Logic();     // T1 - logic
            animateLevel2Objects();  // T3 - animation
        }
    }
    

    updateLighting(); // T3 - lighting
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}


// === MAIN ===
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Game Project");

    initGame();
    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(handleKeyboard);
    glutMouseFunc(handleMouse);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}