//name: Abdel Ngouloure
//Project5_Platformer

#include "Home.h"

#define HOME_WIDTH 14
#define HOME_HEIGHT 8

unsigned int home_data[] =
{
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

void Home::Initialize() {
    //to switch to next scene
    state.nextScene = 1;
    
    GLuint mapTextureID = Util::LoadTexture("landing.png");
    state.map = new Map(HOME_WIDTH, HOME_HEIGHT, home_data, mapTextureID, 1.0f, 4, 1);
    // Move over all of the player and enemy code from initialization.
    
    state.player = new Entity();
    state.player->entityType = PLAYER;
    
    state.player->position = glm::vec3(3.0f, 0.0f, 0);
    
    state.enemies = new Entity();
   state.enemies[1].entityType = ENEMY;
    
}
void Home::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, 1, state.map);
}
void Home::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
}

