#include "Level0.h"

#define LEVEL0_WIDTH 0
#define LEVEL0_HEIGHT 0
#define LEVEL0_ENEMY_COUNT 0

unsigned int level0_data[] = {0};

void Level0::Initialize() {
    
    state.nextScene = -1;
    
    state.map = new Map(LEVEL0_WIDTH, LEVEL0_HEIGHT, level0_data, NULL, 1.0f, 4, 1);
    
    state.player = new Entity();
    state.player->isActive = false;
    
}

void Level0::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL0_ENEMY_COUNT, state.map);
    
    if (state.hasStarted) {
        state.nextScene = 1; 
    }
}

void Level0::Render(ShaderProgram *program) {
    
    state.map->Render(program);
    state.player->Render(program);
    
    Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "MONSTER DASH", 1.1f, -0.26f, glm:: vec3(0.5, -2,0.0f));
    Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "AVOID BEING TOUCHED BY MONSTERS", 0.4f, -0.10f, glm:: vec3(0.65, -4,0.0f));
    
    Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "BUT, YES. YOU MAY JUMP ON THEM.", 0.4f, -0.13f, glm:: vec3(1.2, -4.75,0.0f));
    Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "AVOID FALLING", 0.4f, 0.0f, glm:: vec3(2.5, -5.5,0.0f));
    Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "PRESS ENTER TO BEGIN", 0.6f, -0.20f, glm:: vec3(1.3, -6.5,0.0f));
    
}

