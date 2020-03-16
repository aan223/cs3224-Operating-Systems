#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

#include "Entity.h"

#include <vector>
#define PLATFORM_COUNT 10
#define SUCCESSBLOCK_COUNT 4
#define FIXED_TIMESTEP .016666f
#define RESET 5

struct GameState {
	Entity *player;
	Entity *background;
	Entity *platforms;
	Entity *success;

};

GameState state;
//globals
//GLuint kirbyTextureID;
float LastTicks = 0.0f;
SDL_Window* displayWindow;
bool gameIsRunning = true;
bool start = false;
float accumulator = 0.0f;
float resetTimer = 0;
float accelerationx = 0.25f;


float edgeLeft = -5.00f;
float edgeRight = 5.00f;

float edgeTop = 3.75f;
float edgeBottom = -3.75f;



GLuint fontTextureID;
glm::vec3 fontPos1 = glm::vec3(-1.5f, 2.0f, 0);
glm::vec3 fontPos2 = glm::vec3(-3.25f,0,0);
glm::vec3 fontPos3 = glm::vec3(-2.75f, 1.0f, 0);

std::vector<Entity> collisionChecksPlayer;
std::vector<Entity> fillIn;



ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, modelMatrix2, projectionMatrix;

GLuint LoadTexture(const char* filePath) {
	int w, h, n;
	unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}
	GLuint textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image);
	return textureID;
}

void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position) {
	float width = 1.0f / 16.0f;
	float height = 1.0f / 16.0f;
	std::vector<float> vertices;
	std::vector<float> texCoords;
	for (int i = 0; i < text.size(); i++) {
		int index = (int)text[i];
		float offset = (size + spacing) * i;
		float u = (float)(index % 16) / 16.0f;
		float v = (float)(index / 16) / 16.0f;
		vertices.insert(vertices.end(), { offset + (-0.5f * size), 0.5f * size,
			offset + (-0.5f * size), -0.5f * size,
			offset + (0.5f * size), 0.5f * size,
			offset + (0.5f * size), -0.5f * size,
			offset + (0.5f * size), 0.5f * size,
			offset + (-0.5f * size), -0.5f * size, });
		texCoords.insert(texCoords.end(), { u, v,      
			u, v + height,       
			u + width, v,     
			u + width, v + height,   
			u + width, v,      
			u, v + height, 
			}
		);
	} 
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	program->SetModelMatrix(modelMatrix);
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	
	glBindTexture(GL_TEXTURE_2D, fontTextureID);
	glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
	
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Lunar Landers!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);

	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);

	glUseProgram(program.programID);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	
	
	fontTextureID = LoadTexture("pixel_font.png");


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	state.player = new Entity();
	//state.player->position = glm::vec3(0, 3.25f,0);
	state.player->position = glm::vec3(0, 3.25f, 0);
	state.player->movement = glm::vec3(0);
	state.player->speed = 1.75f;
	state.player->textureID = LoadTexture("Lunar Landing Box SS.png");

	state.player->entityType = PLAYER;
	state.player->flyheight = 4.0f;
	state.player->height = 1.0f;
	state.player->width = 1.0f;

	state.player->animIdle = new int[1]{ 2 };
	state.player -> animSuccess = new int[1]{ 0 };
	state.player->animFail = new int[1]{ 1 };
	state.player->animFrames = 1;
	state.player->animIndices = state.player->animIdle;
	state.player->animCols = 3;
	state.player->animRows = 1;

	state.background = new Entity();
	state.background->textureID = LoadTexture("Sky.png");
	state.background->entityType = BACKGROUND;
	state.background->Update(0, fillIn, 0);
	
	state.platforms = new Entity[PLATFORM_COUNT];
	state.success = new Entity[SUCCESSBLOCK_COUNT];

	GLuint platformTextureID = LoadTexture("Grass Block.png");
	GLuint successTextureID = LoadTexture("Grass Block Success.png");

	state.platforms[0].position = glm::vec3(-4.5f, -3.25f, 0.0f);
	state.platforms[1].position = glm::vec3(-3.5f, -3.25f, 0.0f);
	state.platforms[2].position = glm::vec3(-0.5f, -3.25f, 0.0f);
	state.platforms[3].position = glm::vec3(0.5f, -3.25f, 0.0f);
	state.platforms[4].position = glm::vec3(1.5f, -3.25f, 0.0f);
	state.platforms[5].position = glm::vec3(2.5f, -3.25f, 0.0f);
	state.platforms[6].position = glm::vec3(3.5f, -3.25f, 0.0f);
	state.platforms[7].position = glm::vec3(4.5f, -3.25f, 0.0f);
	state.platforms[8].position = glm::vec3(-4.5f, 1.25f, 0.0f);
	state.platforms[9].position = glm::vec3(-3.5f, 1.25f, 0.0f);


	state.success[0].position = glm::vec3(-2.5f, -3.25f, 0.0f);
	state.success[1].position = glm::vec3(-1.5f, -3.25f, 0.0f);
	state.success[2].position = glm::vec3(2.5f, 1.25f, 0.0f);
	state.success[3].position = glm::vec3(3.5f, 1.25f, 0.0f);
	
	for (int j = 0; j < SUCCESSBLOCK_COUNT; j++) {
		state.success[j].textureID = successTextureID;
		state.success[j].height = 0.8f;
		state.success[j].entityType = SUCCESS_BLOCK;
		state.success[j].Update(0, fillIn, 0); //update once
		collisionChecksPlayer.push_back(state.success[j]);

	}
	
	for (int k = 0; k < PLATFORM_COUNT; k++) {
		state.platforms[k].textureID = platformTextureID;
		state.platforms[k].height = 0.8f;
		state.platforms[k].entityType = PLATFORM;
		state.platforms[k].Update(0, fillIn, 0); //update once
		collisionChecksPlayer.push_back(state.platforms[k]);
	}
	
	
}

void ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {

		state.player->movement = glm::vec3(0);

		switch (event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				start = true;
				state.player->acceleration = glm::vec3(0, -0.25f, 0);
				break;
			}
			
		}
	}



	
	state.player->movement = glm::vec3(0, 0, 0);
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if(start && (!state.player->success || !state.player->fail)){
		if (keys[SDL_SCANCODE_LEFT]) {
			state.player->acceleration.x += -accelerationx;

		}
		else if (keys[SDL_SCANCODE_RIGHT]) {
			state.player->acceleration.x += accelerationx;

		}

		if (glm::length(state.player->movement) > 1.0f) {
			state.player->movement = glm::normalize(state.player->movement);
		}
	}
}




void Update() {
	float tick = (float)SDL_GetTicks() / 1000.f;
	float deltaTime = tick - LastTicks;
	LastTicks = tick;

	deltaTime += accumulator;
	if (deltaTime < FIXED_TIMESTEP) {
		accumulator = deltaTime;
		return;
	}
	while (deltaTime >= FIXED_TIMESTEP) {
		state.player->Update(FIXED_TIMESTEP, collisionChecksPlayer, collisionChecksPlayer.size());
		if (state.player->position.x > edgeRight || state.player->position.x < edgeLeft) {
			state.player->fail = true;
		}
		deltaTime -= FIXED_TIMESTEP;
	}
	accumulator = deltaTime;
	if (state.player->fail || state.player->success) {
		resetTimer += FIXED_TIMESTEP; //why not
		if (resetTimer >= RESET) {
			resetTimer = 0;
			start = false;
			state.player->fail = false;
			state.player->success = false;

			state.player->position = glm::vec3(0, 3.25f, 0);
			state.player->movement = glm::vec3(0);
			state.player->acceleration = glm::vec3(0, 0, 0);
			state.player->velocity = glm::vec3(0, 0, 0);
			state.player->animIndices = state.player->animIdle;
		}
	}
	
}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	
	state.background->Render(&program);
	for (int i = 0; i < SUCCESSBLOCK_COUNT; i++) {
		state.success[i].Render(&program);
	}
	for (int j = 0; j < PLATFORM_COUNT; j++) {
		state.platforms[j].Render(&program);
	}
	
	if (state.player->fail) {
		state.player->animIndices = state.player->animFail;
	}

	else if (state.player->success) {
		state.player->animIndices = state.player->animSuccess;
	}

	state.player->Render(&program);
	if (!start) {
		DrawText(&program, fontTextureID, "Level: 1-1", .2f, 0.1f, fontPos1);
		DrawText(&program, fontTextureID, "Pichu Landers", .3f, 0.1f, fontPos3);
		DrawText(&program, fontTextureID, "Press Space To Start!", .2f, 0.1f, fontPos2);
	}
	else if (state.player->success) {
		DrawText(&program, fontTextureID, "Success!", .3f, 0.1f, fontPos1);
	}
	else if (state.player->fail) {
		DrawText(&program, fontTextureID, "Fail! Try Again", .3f, 0.1f, fontPos1);
	}


	SDL_GL_SwapWindow(displayWindow);


}



void Shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	Initialize();

	while (gameIsRunning) {
		ProcessInput();
		Update();
		Render();
	}

	Shutdown();
	return 0;
}
*/
