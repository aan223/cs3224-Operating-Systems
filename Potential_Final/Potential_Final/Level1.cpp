#include "Level1.h"
#define LEVEL1_WIDTH 15 
#define LEVEL1_HEIGHT 8

#define ENEMY_COUNT 2


unsigned int level1_data[] = { 
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,   
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
	3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 3,
	3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3,
	3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3
};

void Level1::Initialize() {
	//set background

	state.background = new Entity();
	state.background->textureID = Util::LoadTexture("Sky.png");
	state.background->entityType = BACKGROUND;
	state.background->Update(0, state.player, state.enemies, 0, state.map);

	state.prop = new Entity();
	state.prop->textureID = Util::LoadTexture("Pixel Sign.png");
	state.prop->entityType = PROP;
	state.prop->position = glm::vec3(12.5f, -4.0f, 0);
	state.prop->Update(0, state.player, state.enemies, 0, state.map);
	state.prop->isActive = false;

	state.player = new Entity();
	state.player->position = glm::vec3(1, -4.0f, 0);
	state.player->movement = glm::vec3(0);
	//state.player->acceleration = glm::vec3(0, -9.81f, 0);
	state.player->speed = 1.75f;
	state.player->textureID = Util::LoadTexture("Space Slime Player Sprite Sheet.png");

	state.player->entityType = PLAYER;

	state.player->animRight = new int[2]{ 5, 6 };
	state.player->animLeft = new int[2]{ 1, 2 };
	state.player->animUp = new int[2]{ 9, 10 };
	state.player->animDown = new int[2]{ 13, 14 };
	state.player->animIdle = new int[4]{ 3, 7,11,15 };
	state.player->height = 0.95f;
	state.player->width = 0.7f;


	state.player->animFrames = 2;
	state.player->animIndices = state.player->animIdle;
	state.player->animCols = 4;
	state.player->animRows = 4;

	state.enemies = new Entity[ENEMY_COUNT];
	//set up Enemy unit
	

	for (int k = 0; k < ENEMY_COUNT; k++) {
		//Technical 
		//state.enemies[k].acceleration = glm::vec3(0, -9.81f, 0);
		state.enemies[k].position = glm::vec3(0);
		state.enemies[k].movement = glm::vec3(0);
		state.enemies[k].entityType = ENEMY;
		state.enemies[k].speed = 1.0f;
		state.enemies[k].height = 0.9f;
		state.enemies[k].width = 0.8f;

		//Graphics
		state.enemies[k].textureID = Util::LoadTexture("Penguin.png");
		state.enemies[k].animLeft = new int[2]{ 1, 3 };
		state.enemies[k].animRight = new int[2]{ 0,2 };
		state.enemies[k].animIndices = state.enemies[k].animRight;
		state.enemies[k].animFrames = 2;
		state.enemies[k].animCols = 4;
		state.enemies[k].animRows = 1;
	}
	
	state.enemies[0].position = glm::vec3(6.0f, -5.0f, 0);	
	state.enemies[0].enemyType = WALKER;

	state.enemies[1].position = glm::vec3(9.5f, -3.0f, 0);
	state.enemies[1].goLeft = true;
	state.enemies[1].enemyType = WALKER;

	state.lives = new Entity[3];
	for (int i = 0; i < 3; i++) {
		state.lives[i].textureID = Util::LoadTexture("pixel heart.png");
		state.lives[i].entityType = LIVES;
		state.lives[i].position = glm::vec3((1.0f + i*0.75f), -0.5f, 0);
		state.lives[i].Update(0, state.player, state.enemies, 0, state.map);
	
	}



	GLuint mapTextureID = Util::LoadTexture("Winter Blocks.png");
	state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
	state.nextScene = -1;
}
void Level1::Update(float deltaTime) {
	bool complete = false;
	int enemyCount = 0;
	for (int i = 0; i < ENEMY_COUNT; i++) {
		//state.enemies[i].movement.x = 1.0f;
		state.enemies[i].Update(deltaTime, state.player, state.player,1, state.map);
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
	if (state.player-> recover) {
		state.player ->timer += deltaTime;
		if (state.player ->timer >= (deltaTime * 10)) {
			state.player->recover = false;
			state.player ->timer = 0;
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

	if (state.player->position.x >= 12.25f && complete ) {//you have to kill all the enemies before leaving the level
		state.nextScene = 2;
	}
}
void Level1::Render(ShaderProgram *program) { 
	state.background->Render(program);
	state.prop->Render(program);
	state.map->Render(program);
	if (!state.player->fail) {
		for (int i = 0; i < ENEMY_COUNT; i++) {
			state.enemies[i].Render(program);
		}
		for (int j = 0; j < state.player->hp; j++) {
			state.lives[j].Render(program);
		}
		state.player->Render(program);
	}
	else {
		glm::vec3 fontPos2;
		glm::vec3 fontPos3;
		
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

		GLuint fontTextureID = Util::LoadTexture("pixel_font.png");
		Util::DrawText(program, fontTextureID, "Game Over!", .3f, 0.1f, fontPos3);
		Util::DrawText(program, fontTextureID, "Try Again!", .2f, 0.1f, fontPos2);
	}
	
}