#pragma once
#include "StartScreen.h"
#define START_WIDTH 15 
#define START_HEIGHT 8




//All we need here really is the background and text

unsigned int startScreen_data[] = {
	11, 10, 11, 10, 11, 10, 11, 10, 11, 10, 11, 3, 3, 0, 0,
	10, 11, 10, 11, 10, 11, 10, 11, 10, 11, 10, 0, 0, 0, 0,
	11, 10, 11, 10, 11, 10, 11, 10, 11, 10, 11, 0, 0, 0, 0,
	10, 11, 10, 11, 10, 11, 10, 11, 10, 11, 10, 0, 0, 0, 0,
	11, 10, 11, 10, 11, 10, 11, 10, 11, 10, 11, 0, 0, 0, 0,
	10, 11, 10, 11, 10, 11, 10, 11, 10, 11, 10, 0, 0, 0, 0,
	11, 10, 11, 10, 11, 10, 11, 10, 11, 10, 11, 0, 0, 0, 0,
	10, 11, 10, 11, 10, 11, 10, 11, 10, 11, 10, 11, 11, 11, 11
};

void StartScreen::Initialize() {
	//set background

	state.background = new Entity();
	state.background->textureID = Util::LoadTexture("Night Background.png");
	state.background->entityType = BACKGROUND;
	state.background->Update(0, state.player, state.enemies, 0, state.map);

	state.player = new Entity();
	state.player->position = glm::vec3(1, 0, 0);
	state.player->movement = glm::vec3(0);

	state.player->speed = 1.75f;
	state.player->textureID = Util::LoadTexture("Polar Bear Pixel.png");

	state.player->entityType = PLAYER;

	state.player->animRight = new int[2]{ 10, 9 };
	state.player->animLeft = new int[2]{ 3, 4 };
	
	state.player->animIdle = new int[2]{ 5, 11 };

	state.player->height = 0.75f;
	state.player->width = 0.7f;

	state.player->animFrames = 2;
	state.player->animIndices = state.player->animIdle;
	state.player->animCols = 4;
	state.player->animRows = 3;
	state.player->isActive = false;
	
	GLuint mapTextureID = Util::LoadTexture("Space Slimes Background.png");
	state.map = new Map(START_WIDTH, START_HEIGHT, startScreen_data, mapTextureID, 1.0f, 3, 4);
	state.nextScene = -1;
}
void StartScreen::Update(float deltaTime) {
	
}
void StartScreen::Render(ShaderProgram *program) {
	glm::vec3 fontPos2 = glm::vec3(2.0f, -3.0f, 0);
	glm::vec3 fontPos3 = glm::vec3(3.0f, -2.0f, 0);
	GLuint fontTextureID = Util::LoadTexture("pixel_font.png");
	//state.background->Render(program);
	state.map->Render(program);
	Util::DrawText(program, fontTextureID, "Space Slimes", .3f, 0.1f, fontPos3);
	Util::DrawText(program, fontTextureID, "Press Space To Start!", .2f, 0.1f, fontPos2);
	
}