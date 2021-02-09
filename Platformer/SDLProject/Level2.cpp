#include "Level2.h"

#define LEVEL2_WIDTH 21
#define LEVEL2_HEIGHT 8

#define LEVEL2_ENEMY_COUNT 1

unsigned int level2_data[] = {
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3
};

void Level2::Initialize() {
    
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 4, 1);
    
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(7,0,0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0,-9.81f,0); //player always state of free fall
    state.player->speed = 2.0f;
    state.player->textureID = Util::LoadTexture("george_0.png");
    
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};
    
    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.player->height = 0.8f; 
    state.player->width = 0.65f;
    
    state.player->jumpPower = 6.0f;
    
    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("monster.png");
    
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(2,-5,0);
    state.enemies[0].acceleration = glm::vec3(0,-9.81f,0);
    state.enemies[0].speed = 2.0;
    state.enemies[0].aiType = PATROLER;
    
    state.goal = new Entity();
    state.goal->entityType = GOAL;
    state.goal->textureID = Util::LoadTexture("platformPack_tile059.png");
    state.goal->position = glm::vec3(18,-2,0);
    
}

void Level2::Update(float deltaTime) {
    if (state.numOfLives == 0) {
        state.player->gameOver = true;
        for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
            state.enemies[i].gameOver = true;
        }
    }
    
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    state.goal->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    
    for (int i =0; i< LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    }
    
    if (state.player-> CheckCollision(state.goal)) {
        state.nextScene = 3;
    }
    
    if (state.player->hasDied) {
        state.numOfLives -= 1;
        state.player->hasDied = false;
        if (state.numOfLives != 0) {
            state.nextScene = 2;
        }
    }
}

void Level2::Render(ShaderProgram *program) {
    
    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++ ) {
        state.enemies[i].Render(program);
    }
    
    state.player->Render(program);
    state.goal->Render(program);
    
    if (state.player->position.x > 15) {
        if (state.numOfLives == 3) {
            Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "Lives: 3" , 0.3f, 0.0f, glm:: vec3(17.3, -0.5,0.0f));
        }
        else if (state.numOfLives == 2) {
            Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "Lives: 2" , 0.3f, 0.0f, glm:: vec3(17.3, -0.5,0.0f));
        }
        else if (state.numOfLives == 1) {
            Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "Lives: 1" , 0.3f, 0.0f, glm:: vec3(17.3, -0.5,0.0f));
        }
    }
    else if (state.player->position.x > 5) {
        if (state.numOfLives == 3) {
            Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "Lives: 3" , 0.3f, 0.0f, glm:: vec3(state.player->position.x+2.3, -0.5,0.0f));
        }
        else if (state.numOfLives== 2) {
            Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "Lives: 2" , 0.3f, 0.0f, glm:: vec3(state.player->position.x+2.3, -0.5,0.0f));
        }
        else if (state.numOfLives == 1) {
            Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "Lives: 1" , 0.3f, 0.0f, glm:: vec3(state.player->position.x+2.3, -0.5,0.0f));
        }
    }
    else {
        if (state.numOfLives == 3) {
            Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "Lives: 3" , 0.3f, 0.0f, glm:: vec3(7.3, -0.5,0.0f));
        }
        else if (state.numOfLives == 2) {
            Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "Lives: 2" , 0.3f, 0.0f, glm:: vec3(7.3, -0.5,0.0f));
        }
        else if (state.numOfLives== 1) {
            Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "Lives: 1" , 0.3f, 0.0f, glm:: vec3(7.3, -0.5,0.0f));
        }
    }
    
    state.map->Render(program);
    
    
    if (state.numOfLives == 0) {
        if (state.player->position.x > 5.0) {
            Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "YOU LOST", 1.35f, -0.07f, glm:: vec3(state.player->position.x-4.5, -1.5,0.0f));
        }
        else {
            Util::DrawText(program, Util::LoadTexture("pixel_font.png"), "YOU LOST", 1.35f, -0.07f, glm:: vec3(0.6, -1.5,0.0f));
        }
    }
    
}
