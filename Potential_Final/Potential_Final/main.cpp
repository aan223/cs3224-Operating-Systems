//ERICA CHOU 04/17/2020
//PLATFORMER PROJECT 5
//Polar Bear Adventures

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
#include "Level4.h"
#include "Level5.h"
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
Scene* sceneList[7];
int currlvl = 1;
int prevlvl = 1;


//if we complete the whole level, slimes will not respawn
int completed[5] = {0,0,0,0,0};
bool despawnboss = false;



int hp = 3;
void SwitchToScene(Scene *scene) {
	int donecounter = 0;
	currentScene = scene;
	currentScene->Initialize();
	currentScene->state.player->hp = hp;
	if (completed[currlvl - 1] == 1) {
		currentScene->complete = true;
	}
	for (int i = 0; i < 5; i++) {
		if (completed[i] == 1) {
			donecounter++;
		}
	}
	if (currlvl == 1) {// we look at previous levels to see where we spawn
		switch (prevlvl) {
			case 2:
				currentScene->state.player->position = glm::vec3(1.0f, -4.0f, 0);
				break;
			case 3:
				currentScene->state.player->position = glm::vec3(11.0f, -4.0f, 0);
				break;
			case 4:
				currentScene->state.player->position = glm::vec3(7.0f, -1.0f, 0);
				break;
			case 5:
				currentScene->state.player->position = glm::vec3(7.0f, -5.0f, 0);
				break;
		}
	}
	else if (donecounter == 5) {
		currentScene->state.enemies[0].killable = true;
	}
	
}




//globals
float LastTicks = 0.0f;
SDL_Window* displayWindow;
bool gameIsRunning = true;
float accumulator = 0.0f;
float delayTimer = 0.0f;
bool bumpDirectionLeft;
bool start = false;



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

	program.Load("shaders/vertex_lit.glsl", "shaders/fragment_lit.glsl");

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
	sceneList[2] = new Level2();
	sceneList[3] = new Level3();
	sceneList[4] = new Level4();
	sceneList[5] = new Level5();

	SwitchToScene(sceneList[1]);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	music = Mix_LoadMUS("Sector 1.mp3");
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
			    case SDLK_RETURN:
				if (start == false) {
				    start = true;
				    currentScene->state.nextScene = 1;
				    break;
				}
			    case SDLK_SPACE: //Shooting Mechanism
				for(int i = 0; i < 20; i++) {
				    if(currentScene->state.projectile[i].shoot == false) {
                        currentScene->state.projectile[i].shoot = true;
                        break;
				    }
				}
			}
		}

	}

	
	currentScene->state.player->movement = glm::vec3(0, 0, 0);
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_LEFT]) {
		currentScene->state.player->movement.x = -1.0f;
		currentScene->state.player->animIndices = currentScene->state.player->animLeft;
		currentScene->state.player->goLeft = true;
		currentScene->state.player->lastdir = LEFT;
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		currentScene->state.player->movement.x = 1.0f;
		currentScene->state.player->animIndices = currentScene->state.player->animRight;
		currentScene->state.player->goLeft = false;
		currentScene->state.player->lastdir = RIGHT;
	}
	
	if (keys[SDL_SCANCODE_DOWN]) {
		currentScene->state.player->movement.y = -1.0f;
		currentScene->state.player->animIndices = currentScene->state.player->animDown;
		currentScene->state.player->goUp = false;
		currentScene->state.player->lastdir = DOWN;
	}
	
	else if (keys[SDL_SCANCODE_UP]) {
		currentScene->state.player->movement.y = 1.0f;
		currentScene->state.player->animIndices = currentScene->state.player->animUp;
		currentScene->state.player->goUp = true;
		currentScene->state.player->lastdir = UP;
	}
	
	if (glm::length(currentScene->state.player->movement) > 1.0f) {
		currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
	}
	
}




void Update() {
	//Keeps track of different scenes and updates scene based on current scene
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

		program.SetLightPosition(currentScene->state.player->position);

		deltaTime -= FIXED_TIMESTEP;

	}
	if (currentScene->state.enemies[0].killable && (currentScene->state.enemies[0].hp <= 0)) {
		currentScene->state.enemies[0].isActive = false;
		currentScene->state.player->success = true;
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
			if (currentScene->state.nextScene !=0) {
				prevlvl = currlvl;
				currlvl = currentScene->state.nextScene;
				if (currentScene->complete) {
					completed[currlvl - 2] = 1;
				}
				hp = currentScene->state.player->hp;
			}
			SwitchToScene(sceneList[currentScene->state.nextScene]); 
			
		}
		Render();
	}

	Shutdown();
	return 0;
}
