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

#include <SDL_mixer.h>


#include "Map.h"
#include "Entity.h"
#include "Util.h"
#include "Scene.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "StartScreen.h"

//the music is ever changing and based on process inputs so music set up will also be in the main file
#define FIXED_TIMESTEP .016666f


Mix_Music* music;
Mix_Music* success;
Mix_Music* fail;

Mix_Chunk* jump;
Mix_Chunk* bump;
Mix_Chunk* stomp;

Scene* currentScene; 
Scene* sceneList[4];

int hp = 3;
void SwitchToScene(Scene *scene) {
	currentScene = scene;
	currentScene->Initialize();
	currentScene->state.player->hp = hp;
}




//globals
float LastTicks = 0.0f;
SDL_Window* displayWindow;
bool gameIsRunning = true;
bool start = false;
float accumulator = 0.0f;
float delayTimer = 0.0f;
bool bumpDirectionLeft;



ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, modelMatrix2, projectionMatrix;

void Initialize() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	displayWindow = SDL_CreateWindow("Polar Bear Adventures!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	sceneList[0] = new StartScreen();
	sceneList[1] = new Level1();
	//sceneList[2] = new Level2();
	//sceneList[3] = new Level3();
	SwitchToScene(sceneList[0]);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	music = Mix_LoadMUS("Sky Tower.mp3");
	success = Mix_LoadMUS("Victory Kirby.mp3");
	fail = Mix_LoadMUS("Game Over LoZ.mp3");
	Mix_PlayMusic(music, -1);
	Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
	Mix_Volume(-1, MIX_MAX_VOLUME / 4);

	jump = Mix_LoadWAV("smb3_jump.wav");
	bump = Mix_LoadWAV("smb3_bump.wav");
	stomp = Mix_LoadWAV("smb3_stomp.wav");

	
}

void ProcessInput() {
	//takes in input
	SDL_Event event;
	while (SDL_PollEvent(&event)) {

		currentScene->state.player->movement = glm::vec3(0);

		switch (event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				if (start == false) {
					start = true;
					currentScene->state.nextScene = 1;
				}
				break;
			}
		break;
		}

	}

	
	currentScene->state.player->movement = glm::vec3(0, 0, 0);
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_LEFT]) {
		currentScene->state.player->movement.x = -1.0f;
		currentScene->state.player->animIndices = currentScene->state.player->animLeft;
		currentScene->state.player->goLeft = true;
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		currentScene->state.player->movement.x = 1.0f;
		currentScene->state.player->animIndices = currentScene->state.player->animRight;
		currentScene->state.player->goLeft = false;
	}
	
	if (keys[SDL_SCANCODE_DOWN]) {
		currentScene->state.player->movement.y = -1.0f;
		currentScene->state.player->animIndices = currentScene->state.player->animDown;
		currentScene->state.player->goUp = false;
	}
	
	else if (keys[SDL_SCANCODE_UP]) {
		currentScene->state.player->movement.y = 1.0f;
		currentScene->state.player->animIndices = currentScene->state.player->animUp;
		currentScene->state.player->goUp = true;
	}
	
	if (glm::length(currentScene->state.player->movement) > 1.0f) {
		currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
	}
	
}




void Update() {
	//Keeps track of different scenes and updates scene based on current scene
	if (currentScene->state.nextScene >= 0) {
		SwitchToScene(sceneList[currentScene->state.nextScene]);
	}

	float tick = (float)SDL_GetTicks() / 1000.f;
	float deltaTime = tick - LastTicks;
	LastTicks = tick;

	deltaTime += accumulator;

	if (currentScene->state.player->fail && delayTimer == 0) {
		Mix_HaltMusic();
		delayTimer += FIXED_TIMESTEP;
		Mix_PlayMusic(fail, 0);
	}
	if (currentScene->state.player->success && delayTimer == 0) {
		Mix_HaltMusic();
		delayTimer += FIXED_TIMESTEP;
		Mix_PlayMusic(success, 0);
	}
	if ( currentScene->state.player->fail || currentScene->state.player -> success) {
		delayTimer += FIXED_TIMESTEP;
	}
	if (delayTimer >= (FIXED_TIMESTEP * 5000)) {//This number seemed to work well for our purposes
		gameIsRunning = false;
	}


	if (deltaTime < FIXED_TIMESTEP) {
		accumulator = deltaTime;
		return;
	}
	while (deltaTime >= FIXED_TIMESTEP) {
		currentScene->Update(FIXED_TIMESTEP);
		deltaTime -= FIXED_TIMESTEP;

	}

	if (currentScene->state.player->collidedLeft || currentScene->state.player->collidedRight ) {//for player and enemy
		Mix_PlayChannel(-1, bump, 0);
	}

	if ((currentScene->state.player->position.y <= -7.5) && !currentScene->state.player->fail) {
		Mix_PlayChannel(-1, bump, 0);
	}

	if (currentScene->state.player->colLeft || currentScene->state.player->colRight) {//for player and map
			Mix_PlayChannel(-1, bump, 0);
	}

	if (currentScene->state.player->collidedBottom) {
		Mix_PlayChannel(-1, stomp, 0);
	}
	
	accumulator = deltaTime;
	viewMatrix = glm::mat4(1.0f);
	if (currentScene->state.player->position.x > 8.5) {
		viewMatrix = glm::translate(viewMatrix, glm::vec3(-8.5, 3.75, 0));
	}
	else if (currentScene->state.player->position.x > 5 ) {
		viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
	}
	else {
		viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
	}

	
}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	program.SetViewMatrix(viewMatrix);
	currentScene->Render(&program);
	SDL_GL_SwapWindow(displayWindow);


}



void Shutdown() {
	SDL_Quit();
	Mix_FreeChunk(jump);
	Mix_FreeChunk(bump);
	Mix_FreeChunk(stomp);
	Mix_FreeMusic(music);
	Mix_FreeMusic(success);
	Mix_FreeMusic(fail);
}

int main(int argc, char* argv[]) {
	Initialize();

	while (gameIsRunning) {
		ProcessInput();
		Update();
		if (currentScene->state.nextScene >= 0) { 
			if (currentScene->state.nextScene !=1) {
				hp = currentScene->state.player->hp;
			}
			SwitchToScene(sceneList[currentScene->state.nextScene]); 
			
		}
		Render();
	}

	Shutdown();
	return 0;
}
