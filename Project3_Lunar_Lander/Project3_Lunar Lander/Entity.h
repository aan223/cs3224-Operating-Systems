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

#include <vector>
enum EntityType{PLAYER,PLATFORM,SUCCESS_BLOCK,BACKGROUND};

class Entity {
	public:  
		
		int *animIdle = NULL;
		int *animFail = NULL;
		int *animSuccess = NULL;
		int *animIndices = NULL;

		int animFrames = 0;
		int animIndex = 0;
		float animTime = 0.0f;

		int animCols = 0;
		int animRows = 0;

		float width = 1;
		float height = 1;

		bool fly = false;
		float flyheight = 0;
		int maxjumpcount = 2;
		int jumpcount = 0;

		bool idle = true;
		bool isActive =true;

		bool collidedTop = false;
		bool collidedBottom = false;
		bool collidedLeft = false;
		bool collidedRight = false;

		bool fail = false;
		bool success = false;

		EntityType entityType;

		glm::vec3 position;   
		glm::vec3 movement;
		glm::vec3 velocity;
		glm::vec3 acceleration;
		float speed;      
		
		GLuint textureID; 
		
		glm::mat4 modelMatrix;
		
		Entity();       
		
		bool CheckCollision(Entity* other);
		void CheckCollisionsY(std::vector<Entity> objects, int objectCount);
		void CheckCollisionsX(std::vector<Entity> objects, int objectCount);

		void Update(float deltaTime, std::vector<Entity> platform, int platform_count); 
		void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
		void Render(ShaderProgram *program);

};
