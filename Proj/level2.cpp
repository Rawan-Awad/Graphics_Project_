#include <glew.h>
#include "teamHeader.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <cmath>
#include <stdlib.h> // Required for rand()
#include <time.h>   // Required for time()


//#include <cstdlib>
Model_3DS model_car;
Model_3DS model_sign;
Model_3DS model_rock;
Model_3DS model_desert;
Model_3DS model_flag;
Model_3DS model_coin;
const int NUM_COINS= 70;
GameObject coins[NUM_COINS];
const int NUM_ROCKS= 30;
GameObject rocks[NUM_ROCKS];
const int NUM_SIGNS= 30;
GameObject signs[NUM_SIGNS];
bool coinsCollected[NUM_COINS];
bool rocksDestroyed[NUM_ROCKS];
bool signsDestroyed[NUM_SIGNS];

void setupLevel2Objects() {
    // Seed the random number generator. This ensures you get a different
    // layout each time you run the game.
    srand(time(NULL));

    // Define the boundaries for spawning objects.
    // Adjust these values to fit the size of your desert terrain.
    float minX = -50.0f;
    float maxX = 50.0f;
    float minZ = -100.0f; // Start of the level
    float maxZ = -10.0f;  // Near the finish line

    // --- Generate random positions for ROCKS ---
    for (int i = 0; i < NUM_ROCKS; i++) {
        // rand() % 100 gives a number from 0-99. / 100.0f makes it 0.0-0.99.
        // This formula scales and shifts the random number to fit our boundaries.
        rocks[i].x = minX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX - minX)));
        rocks[i].z = minZ + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxZ - minZ)));
        rocks[i].y = 0.5f; // Keep them on the ground
    }

    // --- Generate random positions for SIGNS ---
    for (int i = 0; i < NUM_SIGNS; i++) {
        signs[i].x = minX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX - minX)));
        signs[i].z = minZ + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxZ - minZ)));
        signs[i].y = 0.5f;
    }

    // --- Generate random positions for COINS ---
    for (int i = 0; i < NUM_COINS; i++) {
        coins[i].x = minX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX - minX)));
        coins[i].z = minZ + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxZ - minZ)));
        coins[i].y = 1.0f; // Make coins hover slightly
    }
}

void initLevel2() {
    // Load desert scene, finish line, obstacles
    model_car.Load("Models/car/_Subaru-Loyale.3ds");         // Car model (if different from Level 1)
    model_sign.Load("Models/StopSign/StopSign.3ds");      // Road sign obstacle
    model_rock.Load("Models/rock/Stone 4.3DS");
    model_desert.Load("Models/desert/uploads_files_4614960_Deasert+sell.3ds");
    model_flag.Load("Models/flag/lp_flag3ds.3DS");
    model_coin.Load("Models/coin/uploads_files_4153932_Cartoon_Coin01_3ds.3ds");
    setupLevel2Objects();
    
}


void drawLevel2() {
    // === Draw Desert Terrain ===
    glPushMatrix();
    glScalef(5.0f, 5.0f, 5.0f);
    glTranslatef(0.0f, -17.0f, 0.0f);
   
    model_desert.Draw();
    glPopMatrix();

    // === Draw Player Car ===
    glPushMatrix();
    glTranslatef(player.x, 0.5f, player.z);
    glRotatef(player.yaw, 0.0f, 1.0f, 0.0f);
    glScalef(1.0f, 1.0f, 1.0f);
    model_car.Draw();
    glPopMatrix();

    // === Draw Finish Line Flag ===
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, -70.0f);
    glScalef(3.0f, 3.0f, 3.0f);
    model_flag.Draw();
    glPopMatrix();

    // REMOVED the loops for drawing coins, rocks, and signs from this function.
    // They are now handled by animateLevel2Objects().
}






// In level2.cpp

void animateLevel2Objects() {
    int time = glutGet(GLUT_ELAPSED_TIME);

    // === Animate ALL Rotating Coins ===
    for (int i = 0; i < NUM_COINS; i++) {
        // Only animate the coin if it hasn't been collected
        if (!coinsCollected[i]) {
            glPushMatrix();
            // Use the position from the coins array
            glTranslatef(coins[i].x, coins[i].y, coins[i].z);
            glRotatef(time * 0.1f, 0.0f, 1.0f, 0.0f); // Apply rotation
            glScalef(3.0f, 3.0f, 3.0f); // Use the same scale as in drawLevel2
            model_coin.Draw();
            glPopMatrix();
        }
    }

    // === Animate ALL Pulsing Rocks ===
    for (int i = 0; i < NUM_ROCKS; i++) {
        // +++ ADD THIS CHECK +++
        // Only animate/draw the rock if it has NOT been destroyed.
        if (!rocksDestroyed[i]) {
            glPushMatrix();
            // ... (the rest of the rock animation code is fine) ...
            float scale = 1.0f + 0.1f * sin(time * 0.005f + i);
            glTranslatef(rocks[i].x, rocks[i].y, rocks[i].z);
            glScalef(scale * 3.0f, scale * 3.0f, scale * 3.0f);
            model_rock.Draw();
            glPopMatrix();
        }
    }

    // === Animate ALL Pulsing Signs ===
    for (int i = 0; i < NUM_SIGNS; i++) {
        // +++ ADD THIS CHECK +++
        // Only animate/draw the sign if it has NOT been destroyed.
        if (!signsDestroyed[i]) {
            glPushMatrix();
            // ... (the rest of the sign animation code is fine) ...
            float scale = 1.0f + 0.1f * sin(time * 0.005f + i + NUM_ROCKS);
            glTranslatef(signs[i].x, signs[i].y, signs[i].z);
            glRotatef(-90.0, 0.0f, 1.0f, 0.0f);
            glScalef(scale * 3.0f, scale * 3.0f, scale * 3.0f);
            model_sign.Draw();
            glPopMatrix();
        }
    }
}




void initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

    // === Sun (LIGHT0) initial config ===
    glEnable(GL_LIGHT0);
    GLfloat ambientSun[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuseSun[] = { 0.5f, 0.5f, 0.45f, 1.0f };
    GLfloat specularSun[] = { 0.4f, 0.4f, 0.4f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientSun);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseSun);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularSun);

    // === Headlights (LIGHT1) initial config ===
    glEnable(GL_LIGHT1);
    GLfloat ambientHL[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat diffuseHL[] = { 1.0f, 1.0f, 0.8f, 1.0f };
    GLfloat specularHL[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT1, GL_AMBIENT, ambientHL);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseHL);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specularHL);
}



