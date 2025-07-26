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

GLTexture tex_ground;
GLTexture tex_sky;

float rotationAngle = 0.0f;


void initLevel1() {
    model_car.Load("Models/car/_Subaru-Loyale.3ds");
	city.Load("Models/City/city.3ds");
	coffee.Load("Models/coffee/coffee.3ds");
    hotdog.Load("Models/hotdog/hotdog.3ds");
	fuel.Load("Models/fuelCan/fuelCan.3ds");
	nitro.Load("Models/nitros/oxycan.3ds");

    tex_ground.Load("textures/ground.bmp");  // uses your GLTexture::Load
    tex_sky.Load("textures/blu-sky-3.bmp");
    // Load city models, collectibles, layout
    //model_car.Load("models/car/_Subaru-Loyale.3ds");
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

 

    glPushMatrix();
    glTranslatef(-30.0f, -0.8f, 0.0f);   // ↓ drop city
    glScalef(0.5f, 0.5f, 0.5f);
    city.Draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(+30.0f, -0.8f, 0.0f);
    glScalef(0.5f, 0.5f, 0.5f);
    city.Draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(player.x, player.y, player.z);
    glRotatef(player.yaw, 0, 1, 0);
    glScalef(1.0f, 1.0f, 1.0f);
    model_car.Draw();
    glPopMatrix();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    drawGroundTextured();


    animateLevel1Objects();



    // Draw city, player, obstacles, pickups

}

void animateLevel1Objects() {

    // Advance rotation each frame
    rotationAngle += 1.0f;
    if (rotationAngle > 360.f) rotationAngle -= 360.f;

    // List of (model, X, Z, scale, phase‑offset)
    struct Pickup { Model_3DS* m; float x, z, s, phase; };
    Pickup pickups[] = {
      { &hotdog, -2.0f,  2.0f, 0.03f, 0.0f },
      { &coffee,  2.0f,  2.0f, 1.0f, 1.0f },
      { &nitro,   2.0f, -2.0f, 0.02f, 2.0f },
      { &fuel,   -2.0f, -2.0f, 0.02f, 3.0f },
    };

    const float baseY = 3.5f;  // float 2 units above ground
    const float amplitude = 0.5f;   // 0.5 units of bob

    for (auto& p : pickups) {
        // Compute vertical bob: base + sine wave
        float bob = sinf((rotationAngle * 0.05f + p.phase)) * amplitude;
        float y = baseY + bob;

        glPushMatrix();
        glTranslatef(p.x, y, p.z);
        glRotatef(rotationAngle, 0, 1, 0);
        glScalef(p.s, p.s, p.s);
        p.m->Draw();
        glPopMatrix();
    }
}

void initSounds() {
    // Load chewing, fuel, crash sounds
}
