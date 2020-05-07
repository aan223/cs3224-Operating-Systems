#pragma once
#define GL_SILENCE_DEPRECATION
#ifdef _WINDOWS 
#include <GL/glew.h>
#endif #define GL_GLEXT_PROTOTYPES 1 
#include <SDL.h> 
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp" 
#include "ShaderProgram.h"
#include "Util.h"
#include "Entity.h"
#include "Map.h"

struct GameState {
	Map *map;   
	Entity *player;  
	Entity *enemies;
	Entity* background;
	Entity* prop;
	Entity* lives;
	Entity* projectile;
	int nextScene;
};

class Scene{
	//various globalvariables I think I'm going to need
	 public: 
		 bool complete = true;
		 GameState state;   
		 virtual void Initialize() = 0;  
		 virtual void Update(float deltaTime) = 0;
		 virtual void Render(ShaderProgram *program) = 0;
 };