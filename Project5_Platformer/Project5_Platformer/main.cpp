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
#include <SDL_mixer.h>

#include "Util.h"
//#include "Entity.h"
#include "Map.h"

#include <vector>
#define ENEMY_COUNT 3
//#define PLATFORM_COUNT 21
#define fixed_timestamp .0166666f
#define reset 5

// Add headers
#include "Scene.h"
#include "Home.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, modelMatrix2, projectionMatrix;

// Add some variables and SwitchToScene function
Scene *currentScene;
Scene *sceneList[4];

void SwitchToScene(Scene *scene) {
    currentScene = scene;
    currentScene->Initialize();
}

Mix_Music *music;

//GameState state;
bool start = true;
float LastTicks = 0.0f;
float accumulator = 0.0f;
float resetTimer = 0;
float accelerationx = 0.25f;

float edgeLeft = 3.55f;
float edgeRight = 10.10f;
float edgeTop = 3.75f;
float edgeBottom = -3.75f;

GLuint fontTextureID;
glm::vec3 fontPos3 = glm::vec3(3.0, -2.0f, 0);
glm::vec3 fontPos4 = glm::vec3(1.0, -3.0f, 0);

//std::vector<Entity> collisionCheck;
//std::vector<Entity> fill;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    //Start Audio
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    music = Mix_LoadMUS("gamemusic.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME/4);
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(100.0f, 100.0f, 100.0f, 1.0f);

    glUseProgram(program.programID);
    
    //text texture
    fontTextureID = Util::LoadTexture("font2.png");
    
    glClearColor(.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    sceneList[0] = new Home();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0]);
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
        currentScene->state.player->displacement = glm::vec3(0);
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;
                
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case SDLK_SPACE:
                        if(currentScene->state.player->collidedBottom) {
                            currentScene->state.player->jump = true;
                        }
                    case SDLK_RETURN:
                        if(currentScene->state.nextScene == 1) { //Hit space to start
                            SwitchToScene(sceneList[currentScene->state.nextScene]);
                        }
                }
        }
        
    }

    currentScene->state.player->displacement = glm::vec3(0, 0, 0);
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if(start && (!currentScene->state.player->failure || !currentScene->state.player->victory)){
        if (keys[SDL_SCANCODE_RIGHT]) {
            //state.player->acceleration.x += accelerationx;
            currentScene->state.player->displacement.x = currentScene->state.player->speed;
            currentScene->state.player->animIndices = currentScene->state.player->animRight;
        }
        else if (keys[SDL_SCANCODE_LEFT]) {
            //state.player->acceleration.x += -accelerationx;
            currentScene->state.player->displacement.x = -currentScene->state.player->speed;
            currentScene->state.player->animIndices = currentScene->state.player->animLeft;
        }
        if (glm::length(currentScene->state.player->displacement) > 1.0f) {
            currentScene->state.player->displacement = glm::normalize(currentScene->state.player->displacement);
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
        currentScene->state.player->Update(fixed_timestamp, currentScene->state.player, currentScene->state.enemies, ENEMY_COUNT, currentScene->state.map);
        
        for (int i = 0; i < ENEMY_COUNT; i++) {
            currentScene->state.enemies[i].Update(fixed_timestamp, currentScene->state.player, currentScene->state.player, 1, currentScene->state.map);
        }
        /*
        if (state.player->position.x > edgeRight || state.player->position.x < edgeLeft) {
            state.player->failure = true;
        }
         */
        for(int i = 0; i < ENEMY_COUNT; i++) {
            if (currentScene->state.enemies[i].position.x > edgeRight || currentScene->state.enemies[i].position.x < edgeLeft) {
                currentScene->state.enemies[i].speed*=-1;
            }
            if(currentScene->state.enemies[i].collidedBottom){
                currentScene->state.enemies[i].jump = true;
            }
        }
        deltaTime -= fixed_timestamp;
    }
    accumulator = deltaTime;
    
    viewMatrix = glm::mat4(1.0f);
    if (currentScene->state.player->position.x > 5) {
    viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    } else {
     viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    //Text positioning
    glm::vec3 fontPos1 = glm::vec3(currentScene->state.player->position.x, currentScene->state.player->position.y, 0);
    //DIsplay life count
    glm::vec3 fontPos2 = glm::vec3(currentScene->state.player->position.x - 4, -0.5, 0);
    
    program.SetViewMatrix(viewMatrix);
    
    currentScene->state.map->Render(&program);
    
    currentScene->state.player->Render(&program);
    
    //to determine victory
    currentScene->state.player->victory = true;
    for(int i = 0; i < ENEMY_COUNT; i++) {
        if(currentScene->state.enemies[i].isActive) currentScene->state.player->victory = false;
        currentScene->state.enemies[i].Render(&program);
    }
    //Display on Home Page
    if(currentScene->state.nextScene == 1) {
        TextRendering(&program, fontTextureID, "Platformer", .3f, 0.15f, fontPos3);
        TextRendering(&program, fontTextureID, "Hit Enter to Begin", .3f, 0.15f, fontPos4);
    }
    
    TextRendering(&program, fontTextureID, "LIVES: "+ std::to_string(currentScene->state.player->lives), .3f, 0.15f, fontPos2);
    
    //Display winning text
    if (currentScene->state.player->victory and currentScene->state.nextScene == 4) {
        TextRendering(&program, fontTextureID, "YOU WIN", .3f, 0.15f, fontPos1);
    }
    //Display losing text
    if (currentScene->state.player->failure && currentScene->state.player->lives == 0) {
        TextRendering(&program, fontTextureID, "GAME OVER", .3f, 0.15f, fontPos1);
    }
    else if (currentScene->state.player->failure)
        currentScene->state.player->position = glm::vec3(2.0f, -3.0f, 0);
    //restore failure
    currentScene->state.player->failure = false;

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
        
        //Current life points to keep track of
        currentScene->state.lifeUpdate = currentScene->state.player->lives;
        
        //Condition for switching to next scene
        if (currentScene->state.player->victory and currentScene->state.nextScene != 4)
        SwitchToScene(sceneList[currentScene->state.nextScene]);
        
        Render();
        
    }

    Shutdown();
    return 0;
}
