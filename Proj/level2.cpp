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
        bool positionFound = false;
        while (!positionFound) {
            // a. Generate a potential random position
            float potentialX = minX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX - minX)));
            float potentialZ = minZ + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxZ - minZ)));

            // b. Check its distance against all previously placed obstacles
            bool isTooClose = false;
            for (const auto& placed : placedObstacles) {
                float dx = potentialX - placed.x;
                float dz = potentialZ - placed.z;
                if (sqrt(dx * dx + dz * dz) < minDistance) {
                    isTooClose = true;
                    break; // Found a conflict, no need to check further
                }
            }

            // c. If it's not too close, accept the position
            if (!isTooClose) {
                rocks[i] = { potentialX, 0.5f, potentialZ };
                placedObstacles.push_back(rocks[i]); // Add to our list of placed obstacles
                positionFound = true;
            }
            // d. If it was too close, the while loop repeats, trying a new random spot
        }
    }

    // --- Generate positions for SIGNS (using the same method) ---
    for (int i = 0; i < NUM_SIGNS; i++) {
        bool positionFound = false;
        while (!positionFound) {
            float potentialX = minX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX - minX)));
            float potentialZ = minZ + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxZ - minZ)));

            bool isTooClose = false;
            for (const auto& placed : placedObstacles) {
                float dx = potentialX - placed.x;
                float dz = potentialZ - placed.z;
                if (sqrt(dx * dx + dz * dz) < minDistance) {
                    isTooClose = true;
                    break;
                }
            }

            if (!isTooClose) {
                signs[i] = { potentialX, 0.5f, potentialZ };
                placedObstacles.push_back(signs[i]);
                positionFound = true;
            }
        }
    }

    // --- 3. Simple Random Spawning for COINS ---
    // Coins don't need distance checking, so we can place them freely.
    for (int i = 0; i < NUM_COINS; i++) {
        coins[i].x = minX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxX - minX)));
        coins[i].z = minZ + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxZ - minZ)));
        coins[i].y = 1.0f; // Make coins hover
    }
}

void initLevel2() {
    // Load desert scene, finish line, obstacles
    model_car.Load("Models/car/_Subaru-Loyale.3ds");         // Car model (if different from Level 1)
    model_sign.Load("Models/Sign 1/Sign 1.3ds");      // Road sign obstacle
    model_rock.Load("Models/rock3/Rock07-Base.3ds");
    model_desert.Load("Models/desert/uploads_files_4614960_Deasert+sell.3ds");
    model_flag.Load("Models/flag3/uploads_files_2024783_Flag_v1_001.3DS");
    model_coin.Load("Models/coin2/uploads_files_3504028_Coin.3ds");
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
    glTranslatef(player.x, groundLevelY + 0.5f, player.z);
    glRotatef(player.yaw, 0.0f, 1.0f, 0.0f);
    glScalef(1.0f, 1.0f, 1.0f);
    model_car.Draw();
    glPopMatrix();

    // === Draw Finish Line Flag ===
    glPushMatrix();
    glTranslatef(0.0f, -40.0f, 0.0f);
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
            glTranslatef(coins[i].x, groundLevelY + 1.0f, coins[i].z);
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



