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

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

#include "Entity.h"

#include <vector>
#define victoryblock_num 2
#define surfaceblock_num 13
#define fixed_timestamp .0165432f
#define reset 5

struct GameState {
    Entity *player;
    Entity *surface;
    Entity *victoryblock;
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
float edgeRight = 4.55f;
float edgeTop = 3.75f;
float edgeBottom = -3.75f;

GLuint fontTextureID;
glm::vec3 fontPos1 = glm::vec3(-1.45f, 1.75f, 0);
glm::vec3 fontPos2 = glm::vec3(-3.20f,0,0);
glm::vec3 fontPos3 = glm::vec3(-2.70f, 1.5f, 0);

std::vector<Entity> collisionCheck;
std::vector<Entity> fill;

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
    displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    
    state.player->position = glm::vec3(2.0, 3.25f, 0);
    state.player->displacement = glm::vec3(0);
    state.player->speed = 1.68f;
    state.player->acceleration = glm::vec3(0, -0.20f, 0);
    
    state.player->textureID = LoadTexture("spacheship.png");

    state.player->entityType = PLAYER;
    
    state.player->width = 1.0f;
    state.player->height = 1.0f;

    state.player->animIdle = new int[1]{2};
    state.player->animIndices = state.player->animIdle;
    state.player->animCols = 3;
    state.player->animRows = 1;
    
    state.surface = new Entity[surfaceblock_num];
    state.victoryblock = new Entity[victoryblock_num];
    
    //Ground texture
    GLuint surfaceTextureID = LoadTexture("ground.png");
    GLuint victoryTextureID = LoadTexture("landing.png");

    state.victoryblock[0].position = glm::vec3(-1.5f, -3.25f, 0.0f);
    state.victoryblock[1].position = glm::vec3(2.5f, -2.25f, 0.0f);
    
    state.surface[0].position = glm::vec3(-4.5f, -3.25f, 0.0f);
    state.surface[1].position = glm::vec3(-2.5f, -3.25f, 0.0f);
    state.surface[2].position = glm::vec3(-0.5f, -7.25f, 0.0f);
    state.surface[3].position = glm::vec3(0.5f, -3.25f, 0.0f);
    state.surface[4].position = glm::vec3(1.5f, -3.25f, 0.0f);
    state.surface[5].position = glm::vec3(1.5f, -2.25f, 0.0f);
    state.surface[6].position = glm::vec3(1.5f, -1.25f, 0.0f);
    state.surface[7].position = glm::vec3(4.5f, -3.25f, 0.0f);
    state.surface[8].position = glm::vec3(-4.5f, .25f, 0.0f);
    state.surface[9].position = glm::vec3(-3.5f, .25f, 0.0f);
    state.surface[10].position = glm::vec3(3.5f, .25f, 0.0f);
    state.surface[11].position = glm::vec3(4.5f, 1.25f, 0.0f);
    state.surface[12].position = glm::vec3(-2.0f, 1.25f, 0.0f);
    
    
    for (int i = 0; i < victoryblock_num; i++) {
        state.victoryblock[i].textureID = victoryTextureID;
        state.victoryblock[i].height = 0.8f;
        state.victoryblock[i].entityType = VICTORY_BLOCK;
        state.victoryblock[i].Update(0, fill, 0); //update once
        collisionCheck.push_back(state.victoryblock[i]);
    }
    
    for (int i = 0; i < surfaceblock_num; i++) {
        state.surface[i].textureID = surfaceTextureID;
        state.surface[i].height = 0.5f;
        state.surface[i].entityType = SURFACE;
        state.surface[i].Update(0, fill, 0);
        collisionCheck.push_back(state.surface[i]);
    }
    
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
        }
    }

    state.player->displacement = glm::vec3(0, 0, 0);
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    if(start && (!state.player->failure || !state.player->victory)){
        if (keys[SDL_SCANCODE_RIGHT]) {
            state.player->acceleration.x += accelerationx;
        }
        else if (keys[SDL_SCANCODE_LEFT]) {
            state.player->acceleration.x += -accelerationx;
        }
        if (glm::length(state.player->displacement) > 1.0f) {
            state.player->displacement = glm::normalize(state.player->displacement);
        }
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (int i = 0; i < victoryblock_num; i++) {
        state.victoryblock[i].Render(&program);
    }
    for (int i = 0; i < surfaceblock_num; i++) {
        state.surface[i].Render(&program);
    }
    state.player->Render(&program);
    if (state.player->victory) {
        TextRendering(&program, fontTextureID, "Mission SUccessful", .3f, 0.15f, fontPos1);
    }
    else if (state.player->failure) {
        TextRendering(&program, fontTextureID, "Mission Failed", .3f, 0.15f, fontPos1);
    }

    SDL_GL_SwapWindow(displayWindow);
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
        state.player->Update(fixed_timestamp, collisionCheck, collisionCheck.size());
        if (state.player->position.x > edgeRight || state.player->position.x < edgeLeft) {
            state.player->failure = true;
        }
        deltaTime -= fixed_timestamp;
    }
    accumulator = deltaTime;
    if (state.player->victory || state.player->failure) {
        resetTimer += fixed_timestamp;
        if (resetTimer >= reset) {
            resetTimer = 0;
            state.player->failure = false;
            state.player->victory = false;
            start = false;

            state.player->position = glm::vec3(0, 3.25f, 0);
            state.player->displacement = glm::vec3(0);
            
            state.player->velocity = glm::vec3(0, 0, 0);
            state.player->acceleration = glm::vec3(0, 0, 0);
            
            state.player->animIndices = state.player->animIdle;
        }
    }
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

