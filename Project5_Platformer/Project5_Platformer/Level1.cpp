//name: Abdel Ngouloure
//Project5_Platformer

#include "Level1.h"

//#include <SDL_mixer.h>

//Mix_Music *music;

#define LEVEL1_ENEMY_COUNT 3

#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

unsigned int level1_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3,
    3, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 3,
    3, 1, 1, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3,
};

void Level1::Initialize() {
    //to switch to next scene
    state.nextScene = 2;
    
    GLuint mapTextureID = Util::LoadTexture("landing.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
    // Move over all of the player and enemy code from initialization.
    
    //Start Audio
    //Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    //music = Mix_LoadMUS("gamemusic.mp3");
    //Mix_PlayMusic(music, -1);
    //Mix_VolumeMusic(MIX_MAX_VOLUME/4);
    
    state.player = new Entity();
    state.player->entityType = PLAYER;
    
    state.player->position = glm::vec3(3.0f, 0.0f, 0);
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
    
    //Update current life
    //state.player->lives = state.lifeUpdate;
    
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    for(int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].width = 0.70f;
        state.enemies[i].height = 1.0f;
        state.enemies[0].speed = 1;
    }
    state.enemies[0].position = glm::vec3(8.0f, -4.25f, 0);
    state.enemies[0].acceleration = glm::vec3(0, -0.20f, 0);
    state.enemies[0].aiType = WALKER;
    state.enemies[0].aiState = WALKING;
    
    state.enemies[1].position = glm::vec3(9.0f, -4.25f, 0);
    state.enemies[1].acceleration = glm::vec3(0, -0.20f, 0);
    state.enemies[1].aiType = WALKER;
    state.enemies[1].aiState = WALKING;
    
    state.enemies[2].position = glm::vec3(12.0f, .75f, 0);
    state.enemies[2].acceleration = glm::vec3(0, -1.75f, 0);
    state.enemies[2].aiType = JUMPER;
    //state.enemies[2].aiState = JUMPING;
    state.enemies[2].jumpPower = 2.5f;
}
void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    
    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.player, 1, state.map);
    }
    //move to next level
    //if(state.player->position.x >= 12) {
    //if(state.player->victory) {
    //    state.nextScene = 1;
    //}
}
void Level1::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    
    for(int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        if(state.enemies[i].isActive) state.player->victory = false;
        state.enemies[i].Render(program);
    }
    
}

