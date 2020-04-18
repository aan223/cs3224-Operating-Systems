//name: Abdel Ngouloure
//Project5_Platformer

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
#include "Map.h"

#include <vector>
enum EntityType {PLAYER,PLATFORM,ENEMY};

enum AIType {WALKER, WAITANDGO, JUMPER};
enum AIState {IDLE, WALKING};

class Entity {
    public:
    EntityType entityType;
    AIType aiType;
    AIState aiState;

    glm::vec3 position;
    glm::vec3 displacement;
    float speed;

    glm::vec3 velocity;
    glm::vec3 acceleration;
    
    GLuint textureID;
    glm::mat4 modelMatrix;
    
    float width = 1;
    float height = 1;

    bool jump = false;
    float jumpPower = 0;
    
    bool failure = false;
    bool victory = false;
    int lives = 3;
    
    int *animIdle = NULL;
    int *animRight = NULL;
    int *animLeft = NULL;
    int *animUp = NULL;
    int *animDown = NULL;
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
    
    bool isActive = true;

    Entity(); //constructor
    bool CheckCollision(Entity *other);
    void CheckCollisionsY(Entity *objects, int objectCount);
    void CheckCollisionsX(Entity *objects, int objectCount);
    // In the middle
    void CheckCollisionsX(Map *map);
    void CheckCollisionsY(Map *map);

    void DrawSpriteFromTexture(ShaderProgram *program, GLuint textureID, int index);
    void Render(ShaderProgram *program);
    void Update(float deltaTime, Entity *player, Entity *object, int objectCount, Map *map);

    void AI(Entity *player);
    void AIWalker();
    void AIWaitAndGo(Entity *player);
    void AIJumper();
};
