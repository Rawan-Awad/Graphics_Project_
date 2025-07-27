#include <glew.h>
#include "teamHeader.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <cmath>
#include <vector> 
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
const int NUM_SIGNS= 40;
GameObject signs[NUM_SIGNS];
bool coinsCollected[NUM_COINS];
bool rocksDestroyed[NUM_ROCKS];
bool signsDestroyed[NUM_SIGNS];
float groundLevelY = -10.0f;

void setupLevel2Objects() {
    srand(time(NULL)); // Seed the random number generator

    // --- 1. Define the Expanded Map Boundaries ---
    // You've moved the player back, so let's make the world much bigger.
    float minX = -100.0f;  // A wide but not excessive left/right range
    float maxX = 100.0f;
    float minZ = -100.0f; // The far end of the map
    float maxZ = 280.0f;   // The closest objects can get to the player's start

    // --- 2. Distance-Checked Spawning for Obstacles ---
    std::vector<GameObject> placedObstacles; // A temporary list to track obstacle positions
    float minDistance = 15.0f; // The minimum distance allowed between any two obstacles

    // --- Generate positions for ROCKS ---
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

    // --- 3. Simple Random Spawning for COINS ---
    // Coins don't need distance checking, so we can place them freely.
    for (int i = 0; i < NUM_COINS; i++) {
        coins[i].x = minX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX - minX)));
        coins[i].z = minZ + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxZ - minZ)));
        coins[i].y = 1.0f; // Make coins hover slightly
    }
}

void initLevel2() {
    // Load desert scene, finish line, obstacles
    model_car.Load("Models/car/_Subaru-Loyale.3ds");         // Car model (if different from Level 1)
    model_sign.Load("Models/Sign 1/Sign 1.3ds");      // Road sign obstacle
    model_rock.Load("Models/rock3/Rock07-Base.3ds");
    model_desert.Load("Models/desert/uploads_files_4614960_Deasert+sell.3ds");
    model_flag.Load("Models/flag/lp_flag3ds.3DS");
    model_coin.Load("Models/coin/uploads_files_4153932_Cartoon_Coin01_3ds.3ds");
    setupLevel2Objects();
    
}


void drawLevel2() {
    // === Draw Desert Terrain ===
    glPushMatrix();
    glScalef(3.0f, 3.0f, 3.0f);
    glTranslatef(0.0f, -14.0f, 0.0f);
   
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
            glScalef(2.5f, 2.5f, 2.5f); // Use the same scale as in drawLevel2
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
            glTranslatef(rocks[i].x, groundLevelY + 0.5f, rocks[i].z);
            glScalef(scale * 0.03f, scale * 0.03f, scale * 0.03f);
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
            
            float scale = 1.0f + 0.1f * sin(time * 0.005f + i + NUM_ROCKS);
            glTranslatef(signs[i].x, groundLevelY + 7.0f , signs[i].z);
            glRotatef(0.0, 0.0f, 1.0f, 0.0f);
            glScalef(scale *0.3  , scale * 0.3, scale * 0.3);
            model_sign.Draw();
            glPopMatrix();
        }
    }
}




void initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);  // Normalize normals for scaling

    // === Sun Light ===
    glEnable(GL_LIGHT0);  // Use LIGHT0 for the sun

    GLfloat ambientSun[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuseSun[] = { 0.5f, 0.5f, 0.45f, 1.0f }; // CHANGED from 0.8 to 0.5

    // Reduce the specular highlight intensity as well.
    GLfloat specularSun[] = { 0.4f, 0.4f, 0.4f, 1.0f }; // CHANGED from 0.7 to 0.4
    GLfloat positionSun[] = { 0.0f, 100.0f, 0.0f, 0.0f }; // Directional light

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientSun);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseSun);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularSun);
    glLightfv(GL_LIGHT0, GL_POSITION, positionSun);

    // === Headlights ===
    glEnable(GL_LIGHT1);  // Use LIGHT1 for headlights

    GLfloat ambientHL[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // No ambient from headlights
    // Make the diffuse component very bright for the test
    GLfloat diffuseHL[] = { 1.0f, 1.0f, 0.8f, 1.0f }; // VERY BRIGHT
    GLfloat specularHL[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // BRIGHT HIGHLIGHT


    glLightfv(GL_LIGHT1, GL_AMBIENT, ambientHL);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseHL);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specularHL);

    
}


