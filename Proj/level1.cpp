#include <glew.h>
#include "teamHeader.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <cmath>
#include <vector> 
#include <stdlib.h> // Required for rand()
#include <time.h> 


static Model_3DS model_car;
Model_3DS model_house;
Model_3DS model_coffee;
Model_3DS model_nitro;
Model_3DS lvl2Flag;
Model_3DS tree;

GLTexture tex_ground;
GLTexture tex_sky;

// — How many of each? —
static const int NUM_HOUSES = 20;
static const int NUM_COFFEE = 10;
static const int NUM_TREES = 20; 
static const int NUM_NITRO = 2;


// — Position + state arrays —
GameObject houses[NUM_HOUSES];
GameObject trees[NUM_TREES];
GameObject coffeePickups[NUM_COFFEE];
GameObject nitroPickups[NUM_NITRO];

// — Have we “collected” or “placed” them? —
bool    housesDrawn[NUM_HOUSES];
bool    treeDrawn[NUM_TREES];
bool coffeeCollected[NUM_COFFEE];
bool  nitroCollected[NUM_NITRO];

// — Animation state (reuse your existing) —
static float rotationAngle = 0.0f;

void setUplvl1() {
    srand(time(NULL));
	
    float minX = -100.0f, maxX = 100.0f;
    float minZ = -100.0f, maxZ = 100.0f;

    const float minDistance = 15.0f;

    // 4) Temp list of all placements so far:
    std::vector<GameObject> placed;
    placed.reserve(NUM_HOUSES + NUM_TREES + NUM_COFFEE + NUM_NITRO);

    // Helper lambda to try and place one object with distance checking:
    auto placeOne = [&](GameObject& obj) {
        bool ok = false;
        while (!ok) {
            float x = minX + float(std::rand()) / RAND_MAX * (maxX - minX);
            float z = minZ + float(std::rand()) / RAND_MAX * (maxZ - minZ);
            // check against all placed
            ok = true;
            for (auto& p : placed) {
                float dx = x - p.x;
                float dz = z - p.z;
                if (std::sqrt(dx * dx + dz * dz) < minDistance) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                obj.x = x;
                obj.z = z;
                placed.push_back(obj);
            }
        }
        };

    // 5) Place Houses
    for (int i = 0; i < NUM_HOUSES; ++i) {
        houses[i].y = 0.0f;
        // we set x/z inside placeOne
        placeOne(houses[i]);
        housesDrawn[i] = true;
    }

    // 6) Place Trees
    for (int i = 0; i < NUM_TREES; ++i) {
        trees[i].y = 0.0f;
        placeOne(trees[i]);
        treeDrawn[i] = true;
    }

    // 7) Place Coffee Pickups (hover Y = 1.0)
    for (int i = 0; i < NUM_COFFEE; ++i) {
        coffeePickups[i].y = 1.0f;
        placeOne(coffeePickups[i]);
        coffeeCollected[i] = false;
    }

    // 8) Place Nitro Pickups (hover Y = 1.0)
    for (int i = 0; i < NUM_NITRO; ++i) {
        nitroPickups[i].y = 1.0f;
        placeOne(nitroPickups[i]);
        nitroCollected[i] = false;
    }
}

void initLevel1() {

	// Load models
	model_coffee.Load("models/coffee/coffee.3ds");
    model_car.Load("Models/car/_Subaru-Loyale.3ds");
	model_nitro.Load("models/nitros/nitro.3DS");
	model_house.Load("models/house/house.3ds");
	lvl2Flag.Load("models/lvl2flag/lvl2Flag.3ds");
	tree.Load("models/tree/Tree1.3ds");
    tex_ground.Load("textures/ground.bmp");
    tex_sky.Load("textures/sky.bmp");
    setUplvl1();

}

void drawGroundTextured()
{
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);

    // push it *just* above y=0 so it floats over the city floor
    const float halfSize = 100.0f;   // was 50 or 20, bump it up
    const float tile = 10.0f;
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0); glVertex3f(-halfSize, 0, -halfSize);
    glTexCoord2f(tile, 0); glVertex3f(halfSize, 0, -halfSize);
    glTexCoord2f(tile, tile); glVertex3f(halfSize, 0, halfSize);
    glTexCoord2f(0, tile); glVertex3f(-halfSize, 0, halfSize);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void drawLevel1() {

    drawGroundTextured();
    for (int i = 0; i < NUM_HOUSES; ++i) {
        if (housesDrawn[i]) {
            glPushMatrix();
            glTranslatef(houses[i].x, player.y, houses[i].z);
            glScalef(1.0f, 1.0f, 1.0f);       // half‑size houses
            glRotatef(90.f, 1, 0, 0);
            model_house.Draw();
            glPopMatrix();
        }
    }
    for (int i = 0; i < NUM_TREES; ++i) {
        if (treeDrawn[i]) {
            glPushMatrix();
            glTranslatef(trees[i].x, player.y, trees[i].z);
            glScalef(0.5f, 0.5f, 0.5f);       // half‑size house
            tree.Draw();
            glPopMatrix();
        }
    }

    /*for (int i = 0; i < NUM_COFFEE; ++i) {
        if (!coffeeCollected[i]) {
        glPushMatrix();
        glTranslatef(coffeePickups[i].x, player.y, coffeePickups[i].z);
        glScalef(1.0f, 1.0f, 1.0f);       // half‑size houses
        model_coffee.Draw();
        glPopMatrix();
    }
    }
    for (int i = 0; i < NUM_NITRO; ++i) {
        if (!nitroCollected[i]) {
            glPushMatrix();
            glTranslatef(nitroPickups[i].x, player.y+2.0f, nitroPickups[i].z);
            glScalef(0.01f, 0.01f, 0.01f);       // half‑size houses
            model_nitro.Draw();
            glPopMatrix();
        }
    }*/


    glPushMatrix();
    glTranslatef(75.0f, 0.0f,75.0f);
    glScalef(2.0f, 2.0f, 2.0f);
    lvl2Flag.Draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(player.x, player.y, player.z);
    glRotatef(player.yaw, 0.0f, 1.0f, 0.0f);
    glScalef(1.0f, 1.0f, 1.0f);
    model_car.Draw();
    glPopMatrix();



    // Draw city, player, obstacles, pickups

}

void animateLevel1Objects() {
    int timeMs = glutGet(GLUT_ELAPSED_TIME);
    float seconds = timeMs * 0.001f;          // convert to seconds
    float rotDeg = timeMs * 0.1f;            // your existing spin rate
    float pulseSpeed = 2.0f;                  // cycles per second
    float pulseAmp = 0.2f;

    // === Animate ALL Rotating Coffee Pickups ===
    for (int i = 0; i < NUM_COFFEE; ++i) {
        if (coffeeCollected[i]) continue;

        // Compute a per‑cup phase so they don't all pulse identically:
        float phase = i * 0.5f;

        // Sine‑wave scaling around 1.0
        float scale = 1.0f + pulseAmp * sinf((seconds * pulseSpeed * 2.0f * 3.14159f) + phase);

        glPushMatrix();
        // 1) move to its position
        glTranslatef(coffeePickups[i].x,
            coffeePickups[i].y,
            coffeePickups[i].z);

        // 2) rotate around Y
        glRotatef(rotDeg, 0.0f, 1.0f, 0.0f);

        // 3) pulse scale
        glScalef(scale, scale, scale);

        // 4) draw
        model_coffee.Draw();
        glPopMatrix();
    }

    for (int i = 0; i < NUM_NITRO; i++) {
        // +++ ADD THIS CHECK +++
        if (!nitroCollected[i]) {
            glPushMatrix();
            // Use the position from the coins array
            glTranslatef(nitroPickups[i].x, player.y+2.0F, nitroPickups[i].z);
            glRotatef(timeMs * 0.1f, 0.0f, 1.0f, 0.0f); // Apply rotation
            glScalef(0.01f, 0.01f, 0.01f); // Use the same scale as in drawLevel2
            model_nitro.Draw();
            glPopMatrix();
        }
    }

    // === Animate ALL Rotating Nitro Pickups ===
    /*for (int i = 0; i < NUM_NITRO; ++i) {
        if (!nitroCollected[i]) {
            glPushMatrix();
            // Use the position from the nitroPickups array
            glTranslatef(nitroPickups[i].x,
                nitroPickups[i].y,
                nitroPickups[i].z);
            glRotatef(time * 0.1f, 0.0f, 1.0f, 0.0f);   // Same spin rate
            // Match your nitro scale
            glScalef(0.02f, 0.02f, 0.02f);
            model_nitro.Draw();
            glPopMatrix();
        }
    }*/


    glPushMatrix();
    glTranslatef(player.x, player.y, player.z);
    glRotatef(player.yaw, 0, 1, 0);
    glScalef(1.0f, 1.0f, 1.0f);
    model_car.Draw();
    glPopMatrix();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void initSounds() {
    // Load chewing, fuel, crash sounds
}
