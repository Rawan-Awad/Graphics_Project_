#include <glew.h>
#include "teamHeader.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>


static Model_3DS model_car;
Model_3DS city;
Model_3DS coffee;
Model_3DS hotdog;
Model_3DS fuel;
Model_3DS nitro;


float rotationAngle = 0.0f;


void initLevel1() {
    model_car.Load("Models/car/_Subaru-Loyale.3ds");
	city.Load("Models/City/city.3ds");
	coffee.Load("Models/coffee/coffee.3ds");
    hotdog.Load("Models/hotdog/hotdog.3ds");
	fuel.Load("Models/fuelCan/fuelCan.3ds");
	nitro.Load("Models/nitros/oxycan.3ds");
    // Load city models, collectibles, layout
    //model_car.Load("models/car/_Subaru-Loyale.3ds");
}

void drawLevel1() {
    glPushMatrix();
    glScalef(0.1, 0.1, 0.1);
    city.Draw();
    glPopMatrix();
    // Draw city, player, obstacles, pickups
}

void animateLevel1Objects() {
    rotationAngle += 1.0f; // Adjust speed as needed
    if (rotationAngle > 360.0f) {
        rotationAngle -= 360.0f;
    }

    // Animate collectibles (burger, coffee, nitro)
    glPushMatrix();
    glTranslatef(0.0f, 1.0f + sin(rotationAngle * 0.1f) * 0.2f, 0.0f); // Floating effect
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f); // Rotation
    hotdog.Draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.0f, 1.0f + sin(rotationAngle * 0.1f + 1.0f) * 0.2f, 0.0f); // Floating effect
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f); // Rotation
    coffee.Draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.0f, 1.0f + sin(rotationAngle * 0.1f + 2.0f) * 0.2f, 0.0f); // Floating effect
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f); // Rotation
    nitro.Draw();
    glPopMatrix();

    // Rotate food/fuel, scale obstacles - to be added later
}

void initSounds() {
    // Load chewing, fuel, crash sounds
}
