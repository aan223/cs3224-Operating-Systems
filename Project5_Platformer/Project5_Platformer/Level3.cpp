//name: Abdel Ngouloure
//Project5_Platformer

#include "Level3.h"
#define LEVEL3_ENEMY_COUNT 3

#define LEVEL3_WIDTH 14
#define LEVEL3_HEIGHT 8

unsigned int level3_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 0, 0, 0,
    3, 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 1, 0, 0,
    3, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1,
    3, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

void Level3::Initialize() {
    state.nextScene = 4;
    //Update current life
    
    GLuint mapTextureID = Util::LoadTexture("ground.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);
    // Move over all of the player and enemy code from initialization.
    
    state.player = new Entity();
    state.player->entityType = PLAYER;
    
    state.player->position = glm::vec3(2.0f, 0.0f, 0);
    state.player->displacement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -3.20f, 0);
    state.player->speed = 1.5f;
    
    state.player->textureID = Util::LoadTexture("george_0.png");
    
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};
    
    state.player->width = 0.60f;
    state.player->height = 0.8f;
    
    state.player->jumpPower = 3.5f;

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.player->victory = false;
    
    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    for(int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].width = 0.70f;
        state.enemies[i].height = 1.0f;
        state.enemies[i].speed = 1;
        state.enemies[i].position = glm::vec3(4.0f+i*2, -.75f+i, 0);
        state.enemies[i].acceleration = glm::vec3(0, -1.75f, 0);
        state.enemies[i].aiType = JUMPER;
        //state.enemies[2].aiState = JUMPING;
        state.enemies[i].jumpPower = 2.5f;
    }
    
}
void Level3::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    
    //move to next level
    //if(state.player->position.x >= 12) {
    //if(state.player->victory) {
        //state.nextScene = 1;
    //}
}
void Level3::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
}

