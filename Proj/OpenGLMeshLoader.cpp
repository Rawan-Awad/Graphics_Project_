#define GLUT_DISABLE_ATEXIT_HACK // Disable glut's atexit registration
#include <Windows.h>
#include "teamHeader.h"
#include <glut.h> 
#include <string> 
#include <cmath>       
#include <algorithm> 
#undef exit
using namespace std;

// === GLOBAL DEFINITIONS ===
Player player = { 0.0f, 0.0f, 0.0f, 0.0f, 0, false };
float moveSpeed = 0.5f;
bool isFirstPerson = false;
GameLevel currentLevel = LEVEL1;
//fake collectibles
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
}

// === INPUT ===
void handleKeyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w': case 'W': player.z -= moveSpeed; break;
    case 's': case 'S': player.z += moveSpeed; break;
    case 'a': case 'A': player.x -= moveSpeed; break;
    case 'd': case 'D': player.x += moveSpeed; break;
    case 'l': case 'L':
        currentLevel = LEVEL2; // Instantly switch to Level 2
        player.stamina = 100;
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
    // collision, pickups, stamina, level switch
    //using fake collectibles
    if (!collected) {
        float dx1 = player.x - collectibleX;
        float dz1 = player.z - collectibleZ;
        float dist1 = sqrt(dx1 * dx1 + dz1 * dz1);
        if (dist1 < 2.0f) {
            player.stamina = min(player.stamina + 10, 100.0f);
            player.score += 5;
            collected = true;
        }
    }
    //obstacle logic
    float dx2 = player.x - obstacleX;
    float dz2 = player.z - obstacleZ;
    float dist2 = sqrt(dx2 * dx2 + dz2 * dz2);
    if (dist2 < 2.0f) {
        player.stamina -= 1.0f;
        if (player.stamina <= 0) {
            player.x = 0;
            player.z = 0;
            player.stamina = 100;
        }
    }
    //switching level
    if (player.x > 30.0f) {
        currentLevel = LEVEL2;
        player.x = 0;
        player.z = 0;
    }
}


void updateLevel2Logic() {
    // --- 1. Continuous Stamina Decrease ---
    // Requirement: "The player has to move through the dessert and tries to survive till he reaches the target."
    // This implies a constant challenge, so we'll decrease stamina over time.
    player.stamina -= 0.05f; // Decrease stamina by a small amount each frame. Adjust this value to make it faster/slower.

    // --- 2. Check for Game Over Condition ---
    if (player.stamina <= 0) {
        currentLevel = GAMEOVER; // Change the game state.
        return; // Stop processing the rest of the level logic for this frame.
    }

    // --- 3. Coin Collection Logic ---
    if (!collected) {
        float dx_coin = player.x - collectibleX;
        float dz_coin = player.z - collectibleZ;
        if (sqrt(dx_coin * dx_coin + dz_coin * dz_coin) < 2.0f) {
            player.score += 10;
            collected = true;
            // A potential game mechanic: collecting a coin could restore a bit of stamina.
            // player.stamina += 20.0f; 
        }
    }

    // --- 4. Obstacle Collision Logic ---
    // Colliding with obstacles will now cause a larger, immediate drop in stamina.
    float dx_obstacle = player.x - obstacleX;
    float dz_obstacle = player.z - obstacleZ;
    if (sqrt(dx_obstacle * dx_obstacle + dz_obstacle * dz_obstacle) < 2.0f) {
        player.stamina -= 15.0f; // A significant penalty for hitting an obstacle.
        player.x -= (dx_obstacle / sqrt(dx_obstacle * dx_obstacle + dz_obstacle * dz_obstacle)) * 0.5f;
        player.z -= (dz_obstacle / sqrt(dx_obstacle * dx_obstacle + dz_obstacle * dz_obstacle)) * 0.5f;
    }

    // --- 5. Win Condition ---
    if (player.z < -49.0f) {
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
    glEnable(GL_LIGHT1);

    // The light's position is correct.
    GLfloat light_pos[] = { player.x, player.y + 0.5f, player.z, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, light_pos);

    GLfloat light_dir[] = { 0.0f, -0.5f, -10.0f };
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_dir);

    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 25.0f);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 50.0f);
}




// === SOUND ===
void playSound(const std::string& type) {
    // play sfx by keyword
}

// === DISPLAY ===
void display() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    updateLighting();
    updateCamera();
    if (currentLevel == LEVEL1) drawLevel1();
    else drawLevel2();
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