#include <glew.h>
//#include "teamHeader.h"
#include "TextureBuilder.h"
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
    model_car.Load("models/car/_Subaru-Loyale.3ds");         // Car model (if different from Level 1)
    model_sign.Load("models/sign/StopSign.3ds");      // Road sign obstacle
    model_rock.Load("models/rock/Stone 4.3DS");
    model_desert.Load("models/desert/uploads_files_4614960_Deasert+sell.3ds");
    model_flag.Load("models/flag/lp_flag3ds.3DS");
    model_coin.Load("models/flag/uploads_files_4153932_Cartoon_Coin01_3ds.3ds");
}

void drawLevel2() {
    // Draw sand, coins, player, target
}

void animateLevel2Objects() {
    // Rotate coins, animate sand effects
}

void initLighting() {
    // Setup sun and headlights
}
