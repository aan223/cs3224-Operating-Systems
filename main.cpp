//name: Abdel Ngouloure
//Project1_2D_Scene
//The program creates two structures one rotating the other moving

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



SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program, program1;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
GLuint playerTextureID, playerTextureID1;
float player_x = 0.0f; //First texture
float player_y = 0.0f; //First texture
float rotate_z = 0.0f; //Second Texture
float lastTicks = 0.0f; //Used to calcuate time since last frame


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
	displayWindow = SDL_CreateWindow("Texture Movements and Rotations!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);

	//load shaders shaders to handle textures
	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
	program1.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

	//Loading images
	playerTextureID = LoadTexture("assets/luffy.png");
	playerTextureID1 = LoadTexture("assets/goku.png");


	projectionMatrix = glm::mat4(1.0f);
	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);

	//projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
	projectionMatrix = glm::ortho(-2.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);

	glUseProgram(program.programID);
	glUseProgram(program1.programID);

	//glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			gameIsRunning = false;
		}
	}
}

void Update() { //Animation of textures
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;

	player_x -= 0.1f*deltaTime; //Updated image1 movement

	rotate_z += 45.0f*deltaTime; //Update image2 rotation

}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	projectionMatrix = glm::mat4(1.0f);
	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);

	//projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
	projectionMatrix = glm::ortho(-2.0f, 2.0f, -1.0f, 1.0f, -1.0f, 1.0f);

	//First Texture Image
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f)); //position and size
	modelMatrix = glm::translate(modelMatrix, glm::vec3(player_x, player_y, 0.0f)); //position/movement

	program.SetModelMatrix(modelMatrix);
	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);

	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);
	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, playerTextureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

	
	//Second Texture Image
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.5f, 1.5f)); //position and size
	modelMatrix = glm::translate(modelMatrix, glm::vec3(1.0f, 0.0f, 0.0f)); //position/movement
	modelMatrix = glm::rotate(modelMatrix, glm::radians(rotate_z), glm::vec3(1.0f, 0.0f, 0.0f)); //ROtation


	program1.SetModelMatrix(modelMatrix);
	program1.SetProjectionMatrix(projectionMatrix);
	program1.SetViewMatrix(viewMatrix);

	glVertexAttribPointer(program1.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program1.positionAttribute);
	glVertexAttribPointer(program1.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program1.texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, playerTextureID1);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program1.positionAttribute);
	glDisableVertexAttribArray(program1.texCoordAttribute);


	glEnable(GL_BLEND); //enable blending

	//Good for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
