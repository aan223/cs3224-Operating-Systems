//name: Abdel Ngouloure
//Project3_Lunar_Lander
//The program creates a Lundar Lander game simulation

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
enum EntityType{PLAYER,SURFACE,VICTORY_BLOCK};
class Entity {
    public:
        glm::vec3 position;
        glm::vec3 displacement;
        float speed;
    
        glm::vec3 velocity;
        glm::vec3 acceleration;
        
        GLuint textureID;
        glm::mat4 modelMatrix;
        
        float width = 1;
        float height = 1;
        
        bool failure = false;
        bool victory = false;
        
        int *animIdle = NULL;
        //Check if neccessary
        int *animfailure = NULL;
        int *animvictory = NULL;
        int *animIndices = NULL;
        int animFrames = 0;
        int animIndex = 0;
        float animTime = 0.0f;
        int animCols = 0;
        int animRows = 0;

        bool idle = true;

        bool collidedTop = false;
        bool collidedBottom = false;
        bool collidedLeft = false;
        bool collidedRight = false;
    
        EntityType entityType;

        Entity(); //constructor
        bool CheckCollision(Entity* other);
        void CheckCollisionsY(std::vector<Entity> objects, int objectCount);
        void CheckCollisionsX(std::vector<Entity> objects, int objectCount);

        void DrawSpriteFromTexture(ShaderProgram *program, GLuint textureID, int index);
        void Render(ShaderProgram *program);
        void Update(float deltaTime, std::vector<Entity> surface, int surface_count);
};
