//name: Abdel Ngouloure
//Project4_Rise_of_The_AI
//The program creates a game with enemies with different properties

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
#define ENEMY_COUNT 3
#define PLATFORM_COUNT 21
#define fixed_timestamp .0166666f
#define reset 5

struct GameState {
    Entity *player;
    Entity *platform;
    Entity *enemies;
};

SDL_Window* displayWindow;
GameState state;
bool gameIsRunning = true;
bool start = true;
float LastTicks = 0.0f;
float accumulator = 0.0f;
float resetTimer = 0;
float accelerationx = 0.25f;

float edgeLeft = -4.55f;
float edgeRight = 3.10f;
float edgeTop = 3.75f;
float edgeBottom = -3.75f;

GLuint fontTextureID;
glm::vec3 fontPos1 = glm::vec3(-1.45f, 1.75f, 0);
glm::vec3 fontPos2 = glm::vec3(-3.20f,0,0);
glm::vec3 fontPos3 = glm::vec3(-2.70f, 1.5f, 0);

//std::vector<Entity> collisionCheck;
//std::vector<Entity> fill;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, modelMatrix2, projectionMatrix;

GLuint LoadTexture(const char* filePath) { //Loading the texture
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }

    GLuint textureID; //creating texture
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID); //biding texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image); //setting texture pixel data

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //minmizing
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //magnifying

    stbi_image_free(image); //freeing the image after loading it
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Rise of the AI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    program.SetColor(100.0f, 100.0f, 100.0f, 1.0f);

    glUseProgram(program.programID);
    
    //text texture
    fontTextureID = LoadTexture("font2.png");
    
    state.player = new Entity();
    state.player->entityType = PLAYER;
    
    state.player->position = glm::vec3(-4.0f, -2.0f, 0);
    state.player->displacement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -3.20f, 0);
    state.player->speed = 1.5f;
    
    state.player->textureID = LoadTexture("george_0.png");
    
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};
    
    state.player->width = 0.60f;
    state.player->height = 0.8f;
    
    state.player->jumpPower = 3.5f;

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.platform = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("ground.png");
    
    for (int i = 0; i < PLATFORM_COUNT-10; i++) {
        state.platform[i].entityType = PLATFORM;
        state.platform[i].textureID = platformTextureID;
        state.platform[i].position = glm::vec3(-5.0f + i, -3.25f, 0.0f);
        state.platform[i].width = 0.75f;
        state.platform[i].height = 1.0f;
        state.platform[i].Update(0, NULL, NULL, 0, NULL, 0);
    }
    
    for (int i = 12; i < PLATFORM_COUNT-1; i++) {
        state.platform[i].entityType = PLATFORM;
        state.platform[i].textureID = platformTextureID;
        state.platform[i].position = glm::vec3(-17.0f + i, -.25f, 0.0f);
        //state.platform[i].height = 0.5f;
        state.platform[i].width = 0.75f;
        state.platform[i].height = 1.0f;
        state.platform[i].Update(0, NULL, NULL, 0, NULL, 0);
    }
    
    state.platform[20].entityType = PLATFORM;
    state.platform[20].textureID = platformTextureID;
    state.platform[20].position = glm::vec3(4.15f , -2.25f, 0.0f);
    //state.platform[i].height = 0.5f;
    state.platform[20].width = 0.75f;
    state.platform[20].height = 1.0f;
    state.platform[20].Update(0, NULL, NULL, 0, NULL, 0);
    
    state.enemies = new Entity[ENEMY_COUNT];
    GLuint enemyTextureID = LoadTexture("ctg.png");
    
    for(int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].textureID = enemyTextureID;
        state.enemies[i].width = 0.70f;
        state.enemies[i].height = 1.0f;
        state.enemies[0].speed = 1;
    }
    state.enemies[0].position = glm::vec3(3.0f, -2.25f, 0);
    state.enemies[0].acceleration = glm::vec3(0, -0.20f, 0);
    state.enemies[0].aiType = WALKER;
    state.enemies[0].aiState = WALKING;
    
    state.enemies[1].position = glm::vec3(2.0f, -2.25f, 0);
    state.enemies[1].acceleration = glm::vec3(0, -0.20f, 0);
    state.enemies[1].aiType = WAITANDGO;
    state.enemies[1].aiState = IDLE;
    
    state.enemies[2].position = glm::vec3(-4.0f, .75f, 0);
    state.enemies[2].acceleration = glm::vec3(0, -1.75f, 0);
    state.enemies[2].aiType = JUMPER;
    //state.enemies[2].aiState = JUMPING;
    state.enemies[2].jumpPower = 2.5f;

    
    glClearColor(.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//Deciphering text/font
void TextRendering(ShaderProgram *program, GLuint fontTextureID, std::string text, float size, float spacing, glm::vec3 position) {
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    std::vector<float> vertices;
    std::vector<float> texCoords;
    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float k = (float)(index % 16) / 16.0f;
        float l = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), { offset + (-0.45f * size), 0.45f * size,
            offset + (-0.45f * size), -0.45f * size,
            offset + (0.45f * size), 0.45f * size,
            offset + (0.45f * size), -0.45f * size,
            offset + (0.45f * size), 0.45f * size,
            offset + (-0.45f * size), -0.45f * size, });
        texCoords.insert(texCoords.end(), { k, l, k, l + height, k + width, l, k + width, l + height, k + width, l, k, l + height,});
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

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        state.player->displacement = glm::vec3(0);
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;
                
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case SDLK_SPACE:
                        if(state.player->collidedBottom) {
                            state.player->jump = true;
                        }
                }
        }
        
    }

    state.player->displacement = glm::vec3(0, 0, 0);
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if(start && (!state.player->failure || !state.player->victory)){
        if (keys[SDL_SCANCODE_RIGHT]) {
            //state.player->acceleration.x += accelerationx;
            state.player->displacement.x = state.player->speed;
            state.player->animIndices = state.player->animRight;
        }
        else if (keys[SDL_SCANCODE_LEFT]) {
            //state.player->acceleration.x += -accelerationx;
            state.player->displacement.x = -state.player->speed;
            state.player->animIndices = state.player->animLeft;
        }
        if (glm::length(state.player->displacement) > 1.0f) {
            state.player->displacement = glm::normalize(state.player->displacement);
        }
    }
}

void Update() {
    float tick = (float)SDL_GetTicks() / 1000.f;
    float deltaTime = tick - LastTicks;
    LastTicks = tick;

    deltaTime += accumulator;
    if (deltaTime < fixed_timestamp) {
        accumulator = deltaTime;
        return;
    }
    while (deltaTime >= fixed_timestamp) {
        state.player->Update(fixed_timestamp, state.player, state.platform, PLATFORM_COUNT, state.enemies, ENEMY_COUNT);
        
        for (int i = 0; i < ENEMY_COUNT; i++) {
            state.enemies[i].Update(fixed_timestamp, state.player, state.platform, PLATFORM_COUNT, state.enemies, ENEMY_COUNT);
        }
        /*
        if (state.player->position.x > edgeRight || state.player->position.x < edgeLeft) {
            state.player->failure = true;
        }
         */
        for(int i = 0; i < ENEMY_COUNT; i++) {
            if (state.enemies[i].position.x > edgeRight || state.enemies[i].position.x < edgeLeft) {
                state.enemies[i].speed*=-1;
            }
            if(state.enemies[i].collidedBottom){
                state.enemies[i].jump = true;
            }
        }
        deltaTime -= fixed_timestamp;
    }
    accumulator = deltaTime;
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platform[i].Render(&program);
        
    }
    
    //to determine victory
    state.player->victory = true;
    for(int i = 0; i < ENEMY_COUNT; i++) {
        if(state.enemies[i].isActive) state.player->victory = false;
        state.enemies[i].Render(&program);
    }
    
    state.player->Render(&program);
    
    if (state.player->victory) {
        TextRendering(&program, fontTextureID, "YOU WIN", .3f, 0.15f, fontPos1);
    }
    else if (state.player->failure) {
        TextRendering(&program, fontTextureID, "GAME OVER", .3f, 0.15f, fontPos1);
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
