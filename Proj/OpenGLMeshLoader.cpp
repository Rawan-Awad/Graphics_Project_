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
Player player = { 0.0f, 0.0f, 0.0f, 0.0f, 0,0, false };
float moveSpeed = 0.5f;
bool isFirstPerson = false;
GameLevel currentLevel = LEVEL1;
float sunIntensity = 0.5f;
bool carSoundPlaying = false;
int resumeCarSoundAt = -1;
//fake collectibles
bool collected = false;
bool nitroActive = false;
int nitroEndTime = 0;
float normalSpeed = 0.5f;
float boostedSpeed = 1.0f;


// === INIT ===
void initGame() {
    glEnable(GL_DEPTH_TEST);
    initLevel1();     // Team 2
    initLevel2();     // Team 3
    initSounds();     // Team 2
    initLighting();   // Team 3
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);//allows gl color to affect material
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

static int getFacingIndex(float yaw) {
    // Round yaw/90 to nearest integer, mod 4
    int idx = int(std::round(yaw / 90.0f)) % 4;
    if (idx < 0) idx += 4;
    return idx;
}
// === INPUT ===


// === INPUT ===
void handleKeyboard(unsigned char key, int x, int y) {
    bool moved = false;
    float moveSpeed = nitroActive ? boostedSpeed : normalSpeed;

    // Compute forward vector from yaw
    float rad = player.yaw * (3.14f / 180.0f);
    float fx = sinf(rad) * moveSpeed;
    float fz = -cosf(rad) * moveSpeed;

    // Which way are we facing?
    int facing = getFacingIndex(player.yaw);
    bool sideways = (facing == 1 || facing == 3);  // left or right

    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (nitroActive && currentTime > nitroEndTime) {
        nitroActive = false; // turn off the boost
    }


    switch (key) {
        //   Rotation A/D unchanged
    case 'a': case 'A':
        player.yaw += 90.0f;
        if (player.yaw >= 360.0f) player.yaw -= 360.0f;
        break;

    case 'd': case 'D':
        player.yaw -= 90.0f;
        if (player.yaw < 0.0f) player.yaw += 360.0f;
        break;

        //   Move Forward/Backward with inversion when sideways
    case 'w': case 'W':
        if (sideways) {
            // swap: W moves backward
            player.x -= fx;
            player.z -= fz;
        }
        else {
            // normal: W moves forward
            player.x += fx;
            player.z += fz;
        }
        moved = true;
        break;

    case 's': case 'S':
        if (sideways) {
            // swap: S moves forward
            player.x += fx;
            player.z += fz;
        }
        else {
            // normal: S moves backward
            player.x -= fx;
            player.z -= fz;
        }
        moved = true;
        break;

        //   Change level or quit
    case 'l': case 'L':
        currentLevel = LEVEL2;
        player.stamina = 100;
        player.z = 200.0f;
        player.yaw = 0.0f;
        break;
    case 27:
        ::exit(EXIT_SUCCESS);
        break;
    }

    // Optionally play your movement sound only if actually moved
    if (moved && !carSoundPlaying && resumeCarSoundAt == -1) {
        playSound("car_move");
        carSoundPlaying = true;
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
    // compute a unit?forward vector from yaw
    float aks_r = player.yaw * float(3.14 / 180.0f);
    float r = -aks_r;
    float fx = sinf(r);
    float fz = -cosf(r);

    if (isFirstPerson) {
        // eye is at the car
        float ex = player.x;
        float ey = player.y + 1.0f;
        float ez = player.z;
        // center is a little bit in front of the car
        gluLookAt(ex, ey, ez,
            ex + fx, ey, ez + fz,
            0, 1, 0);
    }
    else {
        // third?person: back off along the negative forward vector
        float ex = player.x - fx * 10.0f;
        float ey = player.y + 5.0f;
        float ez = player.z - fz * 10.0f;
        // look at the car
        gluLookAt(ex, ey, ez,
            player.x, player.y, player.z,
            0, 1, 0);
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

    // === DRINK COLLECTION ===
    for (int i = 0; i < NUM_COFFEE; i++) {
        // Check the new array
        if (!coffeeCollected[i]) {
            float dx = player.x - coffeePickups[i].x;
            float dz = player.z - coffeePickups[i].z;
            if (sqrt(dx * dx + dz * dz) < 2.0f) {
                player.stamina += 10;
                coffeeCollected[i] = true; // Update the new array
                PlaySound(NULL, NULL, 0); // Immediately stop current sound
                playSound("drink");
                carSoundPlaying = false;
                resumeCarSoundAt = glutGet(GLUT_ELAPSED_TIME) + 1000;
            }
        }
    }

    for (int i = 0; i < NUM_NITRO; i++) {
        // Check the new array
        if (!nitroCollected[i]) {
            float dx = player.x - nitroPickups[i].x;
            float dz = player.z - nitroPickups[i].z;
            if (sqrt(dx * dx + dz * dz) < 2.0f) {
                player.stamina += 10;
                nitroCollected[i] = true; // Update the new array
                //PlaySound(NULL, NULL, 0); // Immediately stop current sound
                //playSound("drink");
                //carSoundPlaying = false;
                //resumeCarSoundAt = glutGet(GLUT_ELAPSED_TIME) + 1000;
                nitroActive = true;
                nitroEndTime = glutGet(GLUT_ELAPSED_TIME) + 10000; // 10 seconds later

            }
        }
    }

    for (int i = 0; i < NUM_HOUSES; i++) {
        float dx = player.x - houses[i].x;
        float dz = player.z - houses[i].z;
        float distance = sqrt(dx * dx + dz * dz);
        float collisionRadius = 6.0f; // The "size" of the player and rock combined

        if (distance < collisionRadius) {
            // A collision has occurred.
            if (player.yaw == -90)
                player.x -= 10; // Push back a bit more if facing left
            else if (player.yaw == 90)
                player.x += 10; // Push back a bit more if facing right
            else if (player.yaw == 180)
                player.z -= 10; // Push back a bit more if facing backward
            else player.z += 10; // Push back a bit more if facing forward
            // A very small stamina penalty for the bump.
            PlaySound(NULL, NULL, 0); // Immediately stop current sound

            playSound("car_crash");
            carSoundPlaying = false;
            resumeCarSoundAt = glutGet(GLUT_ELAPSED_TIME) + 1000;


            // --- Solid Collision Response ---
            // Calculate how much the player has penetrated the obstacle.
            float overlap = collisionRadius - distance;

            // Push the player back along the vector from the obstacle's center
            // to the player's center, just enough to resolve the overlap.
            player.x += (dx / distance) * overlap;
            player.z += (dz / distance) * overlap;
        }
    }
    for (int i = 0; i < NUM_TREES; i++) {
        float dx = player.x - trees[i].x;
        float dz = player.z - trees[i].z;
        float distance = sqrt(dx * dx + dz * dz);
        float collisionRadius = 1.0f; // The "size" of the player and rock combined

        if (distance < collisionRadius) {
            // A collision has occurred.
            if (player.yaw == -90)
                player.x -= 10; // Push back a bit more if facing left
            else if (player.yaw == 90)
                player.x += 10; // Push back a bit more if facing right
            else if (player.yaw == 180)
                player.z -= 10; // Push back a bit more if facing backward
            else player.z += 10; // Push back a bit more if facing forward
            // A very small stamina penalty for the bump.
            PlaySound(NULL, NULL, 0); // Immediately stop current sound

            playSound("car_crash");
            carSoundPlaying = false;
            resumeCarSoundAt = glutGet(GLUT_ELAPSED_TIME) + 1000;

            // --- Solid Collision Response ---
            // Calculate how much the player has penetrated the obstacle.
            float overlap = collisionRadius - distance;

            // Push the player back along the vector from the obstacle's center
            // to the player's center, just enough to resolve the overlap.
            player.x += (dx / distance) * overlap;
            player.z += (dz / distance) * overlap;
        }
    }
    


    // === LEVEL TRANSITION ===
    if (player.x > 75.0f) {
        player.x = 0.0f;
        player.z = 200.0f;
        currentLevel = LEVEL2;
		//player.stamina = 100; // Reset stamina for the new level
        player.yaw = 0.0f;
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

                // Stop car sound and schedule it to resume after 1 second
                PlaySound(NULL, NULL, 0); // Immediately stop current sound
                playSound("collect_coin");
                carSoundPlaying = false;
                resumeCarSoundAt = glutGet(GLUT_ELAPSED_TIME) + 1000; // 1 sec from now
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

            player.stamina -= 0.5f; // A very small stamina penalty for the bump.
            if (player.yaw == -90)
                player.x -= 10; // Push back a bit more if facing left
            else if (player.yaw == 90)
                player.x += 10; // Push back a bit more if facing right
            else if (player.yaw == 180)
                player.z -= 10; // Push back a bit more if facing backward
            else player.z += 10; // Push back a bit more if facing forward

            //player.stamina -= 0.1f; // A very small stamina penalty for the bump.

            PlaySound(NULL, NULL, 0); // Immediately stop current sound

            playSound("car_crash");
            carSoundPlaying = false;
            resumeCarSoundAt = glutGet(GLUT_ELAPSED_TIME) + 1000; // 1 sec from now
            // --- Solid Collision Response ---
            // Calculate how much the player has penetrated the obstacle.
            float overlap = collisionRadius - distance;

            // Push the player back along the vector from the obstacle's center
            // to the player's center, just enough to resolve the overlap.
            player.x += (dx / distance) * overlap;
            player.z += (dz / distance) * overlap;
        }
    }

    //check collision for signs
    for (int i = 0; i < NUM_SIGNS; i++) {
        float dx = player.x - signs[i].x;
        float dz = player.z - signs[i].z;
        float distance = sqrt(dx * dx + dz * dz);
        float collisionRadius = 2.0f; // Using the same collision size

        if (distance < collisionRadius) {
            // Apply the same effects as hitting a rock

            player.stamina -= 0.5f;
			if (player.yaw == -90)
				player.x -= 10; // Push back a bit more if facing left
			else if (player.yaw == 90)
				player.x += 10; // Push back a bit more if facing right
			else if (player.yaw == 180)
				player.z -= 10; // Push back a bit more if facing backward
			else player.z += 10; // Push back a bit more if facing forward
            

            //player.stamina -= 0.1f;

            PlaySound(NULL, NULL, 0); // Immediately stop current sound

            playSound("car_crash");
            carSoundPlaying = false;
            resumeCarSoundAt = glutGet(GLUT_ELAPSED_TIME) + 1000; // 1 sec from now
            // Apply the same solid push-back response
            float overlap = collisionRadius - distance;
            player.x += (dx / distance) * overlap;
            player.z += (dz / distance) * overlap;
        }
    }

    // --- 5. Win Condition ---
    if (player.z <= 0.0f) {
        // Stop current sound and play cheer
        PlaySound(NULL, NULL, 0);
        playSound("win");

        carSoundPlaying = false; // prevent car sound from restarting
        resumeCarSoundAt = 0;   // cancel pending resume

        currentLevel = GAMEWIN;
        return;
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
void drawGameWinScreen() {
    // Switch to 2D orthographic projection to draw text on the screen.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 600, 0); // Using 800x600 window size. Top-left is (0,0).

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Disable 3D features like lighting and depth testing to ensure the UI draws correctly.
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    // Draw a semi-transparent blue background to give a celebratory feel.
    glColor4f(0.0f, 0.2f, 0.5f, 0.5f); // Blue with 50% transparency
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(800, 0);
    glVertex2i(800, 600);
    glVertex2i(0, 600);
    glEnd();

    // Draw the "YOU WIN!" text in the center of the screen.
    glColor3f(1.0f, 1.0f, 0.0f); // Bright yellow color for the text.
    glRasterPos2i(350, 300); // Position the text.
    const char* message = "YOU WIN!";
    for (const char* c = message; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }

    // Draw the "Final Score" text just below the main message.
    std::string scoreMsg = "Final Score: " + std::to_string(player.score);
    glRasterPos2i(330, 340); // Positioned below the "YOU WIN!" text.
    for (char c : scoreMsg) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // Re-enable the 3D features before exiting the function.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    // Restore the previous projection and modelview matrices.
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}



// === LIGHTING ===
void updateLighting() {
    // === SUNLIGHT (Directional Light - Cycles Like Day/Night) ===
    glEnable(GL_LIGHT0);

    // Time-based sine wave: value oscillates between 0.1 and 1.0
    float t = glutGet(GLUT_ELAPSED_TIME) * 0.0001f;  // slower cycle
    sunIntensity = 0.55f + 0.45f * sin(t);  // oscillates between ~0.1 to 1.0
    sunIntensity = max(0.1f, sunIntensity); // don't go fully dark

    GLfloat diffuseSun[] = { sunIntensity, sunIntensity, sunIntensity, 1.0f };
    GLfloat sunDir[] = { -0.3f, -1.0f, -0.5f, 0.0f }; // Directional sunlight
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseSun);
    glLightfv(GL_LIGHT0, GL_POSITION, sunDir);

    // Ambient stays low but constant (general background light)
    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);


    glEnable(GL_LIGHT1);
    // HEADLIGHTS: position in front of car based on yaw
    float rad = player.yaw * (3.14f / 180.0f);
    float headlightX = player.x + sin(rad) * 2.0f;   // 2 units in front
    float headlightY = player.y + 1.0f;              // slightly above car
    float headlightZ = player.z - cos(rad) * 2.0f;

    GLfloat light_pos[] = { headlightX, headlightY, headlightZ, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, light_pos);

    // Direction should point slightly downward and forward
    GLfloat light_dir[] = { sin(rad), -0.2f, -cos(rad) };
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
        PlaySound(TEXT("cheer_2.wav"), NULL, SND_FILENAME | SND_ASYNC);
    }
}

// === DISPLAY ===
void display() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    updateLighting();
    updateCamera();
    if (currentLevel == LEVEL1) { drawLevel1(); 
	animateLevel1Objects(); // <-- ADD THIS CALL
    }
    else {
        drawLevel2();
        animateLevel2Objects();
    }
    if (currentLevel == GAMEOVER) {
        drawGameOverScreen();
    }
    else if (currentLevel == GAMEWIN) {
        drawGameWinScreen(); // <-- ADD THIS CALL
    }


    drawHUD();
    glutSwapBuffers();
}

// === TIMER ===
void timer(int value) {
    if (currentLevel != GAMEOVER && currentLevel != GAMEWIN) {
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
    if (resumeCarSoundAt > 0 && glutGet(GLUT_ELAPSED_TIME) >= resumeCarSoundAt) {
        playSound("car_move");
        carSoundPlaying = true;
        resumeCarSoundAt = -1; // reset timer
    }
    
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
    glutFullScreen();

    glutMainLoop();
    return 0;
}