#include <glew.h>
#include "teamHeader.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>

//#include <cstdlib>
Model_3DS model_car;
Model_3DS model_sign;
Model_3DS model_rock;
Model_3DS model_desert;
Model_3DS model_flag;
Model_3DS model_coin;

void initLevel2() {
    // Load desert scene, finish line, obstacles
    model_car.Load("Models/car/_Subaru-Loyale.3ds");         // Car model (if different from Level 1)
    model_sign.Load("Models/StopSign/StopSign.3ds");      // Road sign obstacle
    model_rock.Load("Models/rock/Stone 4.3DS");
    model_desert.Load("Models/desert/uploads_files_4614960_Deasert+sell.3ds");
    model_flag.Load("Models/flag/lp_flag3ds.3DS");
    model_coin.Load("Models/coin/uploads_files_4153932_Cartoon_Coin01_3ds.3ds");
}

void drawLevel2() {
    // === Draw Desert Terrain ===
    glPushMatrix();
    glScalef(5.0f, 5.0f, 5.0f);
    glTranslatef(0.0f, -20.0f, 0.0f);
    model_desert.Draw();
    glPopMatrix();

    // === Draw Player Car ===
    glPushMatrix();
    glTranslatef(player.x, player.y + 0.5f, player.z);
    // Increase the car's scale to make it visible and proportional to the scene.
    // Let's try making it 5 times bigger than it was before.
    glScalef(1.0f, 1.0f, 1.0f); // CHANGED FROM 0.1 to 0.5
    model_car.Draw();
    glPopMatrix();

    // === Draw Coin (if not collected) ===
    if (!collected) {
        glPushMatrix();
        glTranslatef(collectibleX, 1.0f, collectibleZ);
        glScalef(0.5f, 0.5f, 0.5f);
        model_coin.Draw();
        glPopMatrix();
    }

    // === Draw Rock Obstacle ===
    glPushMatrix();
    glTranslatef(obstacleX, 0.5f, obstacleZ);
    glScalef(0.5f, 0.5f, 0.5f);
    model_rock.Draw();
    glPopMatrix();

    // === Draw Road Sign ===
    glPushMatrix();
    glTranslatef(obstacleX + 5.0f, 0.5f, obstacleZ - 5.0f);
    glScalef(0.5f, 0.5f, 0.5f);
    model_sign.Draw();
    glPopMatrix();

    // === Draw Finish Line Flag ===
    glPushMatrix();
   
    glScalef(0.5f, 0.5f, 0.5f);
    model_flag.Draw();
    glPopMatrix();
}






void animateLevel2Objects() {
    int time = glutGet(GLUT_ELAPSED_TIME);

    // === Animate Rotating Coin ===
    if (!collected) {
        glPushMatrix();
        glTranslatef(collectibleX, 0.0f, collectibleZ);
        glRotatef(time * 0.1f, 0.0f, 1.0f, 0.0f); // Smooth Y rotation
        glScalef(0.05f, 0.05f, 0.05f);
        model_coin.Draw();
        glPopMatrix();
    }

    // === Animate Obstacle (Rock) Pulsing ===
    glPushMatrix();
    float scale = 0.1f + 0.02f * sin(time * 0.005f); // Pulsing animation
    glTranslatef(obstacleX, 0.0f, obstacleZ);
    glScalef(scale, scale, scale);
    model_rock.Draw();
    glPopMatrix();

    // === Animate Second Obstacle (Sign) Pulsing (Optional) ===
    glPushMatrix();
    float scale2 = 0.1f + 0.02f * sin(time * 0.005f + 1); // Slight phase shift
    glTranslatef(obstacleX + 5.0f, 0.0f, obstacleZ - 5.0f);
    glScalef(scale2, scale2, scale2);
    model_sign.Draw();
    glPopMatrix();
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

    // Position will be updated every frame in updateLighting()
}


