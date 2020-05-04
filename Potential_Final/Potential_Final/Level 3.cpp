#include "Level3.h"
#define LEVEL3_WIDTH 15 
#define LEVEL3_HEIGHT 8

#define ENEMY_COUNT 1

//BOSS LEVEL

unsigned int level3_data[] = {
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,3,
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,3,
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,3,
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,3,
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,3,
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,3,
	3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,3,
	3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,3
};

void Level3::Initialize() {
	//set background

	state.background = new Entity();
	state.background->textureID = Util::LoadTexture("Night Background.png");
	state.background->entityType = BACKGROUND;
	state.background->Update(0, state.player, state.enemies, ENEMY_COUNT, state.map);

	state.prop = new Entity();
	state.prop->textureID = Util::LoadTexture("Pixel Sign.png");
	state.prop->entityType = PROP;
	state.prop->position = glm::vec3(12.5f, -5.0f, 0);
	state.prop->Update(0, state.player, state.enemies, 0, state.map);
	state.prop->isActive = false;

	state.player = new Entity();
	state.player->position = glm::vec3(1, -4.0f, 0);
	state.player->movement = glm::vec3(0);
	state.player->acceleration = glm::vec3(0, -9.81f, 0);
	state.player->speed = 1.75f;
	state.player->textureID = Util::LoadTexture("Polar Bear Pixel.png");

	state.player->entityType = PLAYER;

	state.player->animRight = new int[2]{ 10, 9 };
	state.player->animLeft = new int[2]{ 3, 4 };
	//state.player->animRightFly = new int[2]{ 8, 8 };
	//state.player->animLeftFly = new int[2]{ 2, 2 };
	state.player->animIdle = new int[2]{ 5, 11 };
	//state.player->animCharm = new int[2]{1,7};
	//state.player->animDeath = new int[2]{ 0,6 };
	//state.player->flyheight = 6.0f;
	state.player->height = 0.85f;
	state.player->width = 0.7f;


	state.player->animFrames = 2;
	state.player->animIndices = state.player->animIdle;
	state.player->animCols = 4;
	state.player->animRows = 3;

	state.enemies = new Entity[ENEMY_COUNT];
	//set up Enemy unit


	for (int k = 0; k < ENEMY_COUNT; k++) {
		//Technical 
		state.enemies[k].acceleration = glm::vec3(0, -9.81f, 0);
		state.enemies[k].position = glm::vec3(0);
		state.enemies[k].movement = glm::vec3(0);
		state.enemies[k].entityType = ENEMY;
		state.enemies[k].speed = 2.25f;
		state.enemies[k].height = 0.9f;
		state.enemies[k].width = 0.8f;

		//Graphics
		state.enemies[k].textureID = Util::LoadTexture("Penguin Boss.png");
		state.enemies[k].animLeft = new int[2]{ 1, 3 };
		state.enemies[k].animRight = new int[2]{ 0,2 };
		state.enemies[k].animIndices = state.enemies[k].animRight;
		state.enemies[k].animFrames = 2;
		state.enemies[k].animCols = 4;
		state.enemies[k].animRows = 1;
	}

	state.enemies[0].position = glm::vec3(12.0f, -5.0f, 0);
	state.enemies[0].enemyType = STALKER; //the BOSS

	state.lives = new Entity[3];
	for (int i = 0; i < 3; i++) {
		state.lives[i].textureID = Util::LoadTexture("pixel heart.png");
		state.lives[i].entityType = LIVES;
		state.lives[i].position = glm::vec3((1.0f + i * 0.75f), -0.5f, 0);
		state.lives[i].Update(0, state.player, state.enemies, 0, state.map);

	}

	GLuint mapTextureID = Util::LoadTexture("Winter Blocks.png");
	state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);
	state.nextScene = -1;
}
void Level3::Update(float deltaTime) {
	bool complete = false;
	int enemyCount = 0;
	for (int i = 0; i < ENEMY_COUNT; i++) {
		//state.enemies[i].movement.x = 1.0f;
		state.enemies[i].Update(deltaTime, state.player, state.player, 1, state.map);
		if (!state.enemies[i].isActive) {
			enemyCount++;
		}
		if (enemyCount == ENEMY_COUNT) {
			complete = true;
			state.prop->isActive = true;
		}
	}

	if (state.player->recover) {
		state.player->Update(deltaTime, state.player, state.enemies, 0, state.map);
	}
	else {
		state.player->Update(deltaTime, state.player, state.enemies, ENEMY_COUNT, state.map);
	}
	if (state.player->recover) {
		state.player->timer += deltaTime;
		if (state.player->timer >= (deltaTime * 10)) {
			state.player->recover = false;
			state.player->timer = 0;
			state.enemies[0].position = glm::vec3(6.0f, -5.0f, 0);
			state.enemies[1].position = glm::vec3(9.5f, -4.0f, 0);
		}
	}

	//for moving the hearts:
	for (int j = 0; j < state.player->hp; j++) {
		if (state.player->position.x > 8.5) {
			state.lives[j].position = glm::vec3((4.25f + j * 0.75f), -0.5f, 0);
		}
		else if (state.player->position.x > 5) {
			state.lives[j].position = glm::vec3(((state.player->position.x) - 4.25f) + (j * 0.75f), -0.5f, 0);
		}
		else {
			state.lives[j].position = glm::vec3((0.75f + j * 0.75f), -0.5f, 0);
		}
		state.lives[j].Update(0, state.player, state.enemies, 0, state.map);
	}

	if (state.player->position.x >= 12.25f && complete) {//you have to kill all the enemies before leaving the level
		state.player ->success = true;
	}
}
void Level3::Render(ShaderProgram *program) {
	glm::vec3 fontPos2;
	glm::vec3 fontPos3;
	state.background->Render(program);
	state.prop->Render(program);
	state.map->Render(program);
	if(state.player->fail) {
		if (state.player->position.x > 8.5) {
			fontPos2 = glm::vec3(6.25f, -3.0f, 0);
			fontPos3 = glm::vec3(6.0f, -2.0f, 0);
		}
		else if (state.player->position.x > 5) {
			fontPos2 = glm::vec3(state.player->position.x - 1.5f, -3.0f, 0);
			fontPos3 = glm::vec3(state.player->position.x - 1.75f, -2.0f, 0);
		}
		else {
			fontPos2 = glm::vec3(2.75f, -3.0f, 0);
			fontPos3 = glm::vec3(2.5f, -2.0f, 0);
		}

		GLuint fontTextureID = Util::LoadTexture("pixel_font2.png");
		Util::DrawText(program, fontTextureID, "Game Over!", .3f, 0.1f, fontPos3);
		Util::DrawText(program, fontTextureID, "Try Again!", .2f, 0.1f, fontPos2);
	}
	else if (state.player->success) {
		state.player->Render(program);
		glm::vec3 fontPos2 = glm::vec3(7.5f, -3.0f, 0);
		glm::vec3 fontPos3 = glm::vec3(5.5f, -2.0f, 0);
		GLuint fontTextureID = Util::LoadTexture("pixel_font2.png");
		Util::DrawText(program, fontTextureID, "Congratulations!", .3f, 0.1f, fontPos3);
		Util::DrawText(program, fontTextureID, "You Won!", .2f, 0.1f, fontPos2);
	}
	else {
		for (int i = 0; i < ENEMY_COUNT; i++) {
			state.enemies[i].Render(program);
		}
		for (int j = 0; j < state.player->hp; j++) {
			state.lives[j].Render(program);
		}
		state.player->Render(program);
	}
}
