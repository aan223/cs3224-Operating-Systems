//name: Abdel Ngouloure
//Project2_Pong
//The program creates a gameplay of pong

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


class Entity {
public:
	float posX;
	float posY;
	float height;
	float width;

	float speed;
	float xDirection = 0.0f;
	float yDirection = 0.0f;
	float circle;

	GLuint textureID;

	void Render(ShaderProgram &program);
	void displacement(float elapse);
};

//Handles movement of Entity
void Entity::displacement(float elapsed) {
	this->posX += (elapsed*this->xDirection*this->speed);
	this->posY += (elapsed*this->yDirection*this->speed);
}

//Renders Entity 
void Entity::Render(ShaderProgram &program) {
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(posX, posY, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(width, height, 1.0f));
	program.SetModelMatrix(modelMatrix);

	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	glBindTexture(GL_TEXTURE_2D, textureID);

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);
	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);
}

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Entity paddle, paddle1, ball, topBar, bottomBar;
int winScore = 3;
int score = 0;
int score1 = 0;
const Uint8 *keys = SDL_GetKeyboardState(NULL);

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

//reset game after scoring
void reset(Entity &paddle, Entity &paddle1, Entity &ball) {
	paddle.posX = -1.77f;
	paddle.posY = 0.0f;

	paddle1.posX = 1.77f;
	paddle1.posY = 0.0f;

	ball.speed+= .05f;
	ball.posX = 0.0f;
	ball.posY = 0.0f;
	ball.xDirection = 0.0f;
	ball.yDirection = 0.0f;
	
	int rand_y = rand() % 15 - 7;
	ball.yDirection = rand_y / 11.0f;
	int rand_x = rand() % 10;
	ball.xDirection = 1.05f ? -1.05f : rand_x >= 5;
}


void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Pong Gameplay", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);

	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

	ball.textureID = LoadTexture("assets/ball1.png"); //load text 

	projectionMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);
	viewMatrix = glm::mat4(1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);

	glUseProgram(program.programID);


	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-2.0f, 2.0f, -1.75f, 1.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	glUseProgram(program.programID);

	//paddle attribute definition
	paddle.speed = 0.1f;
	paddle.posX = -1.77f;
	paddle.posY = 0.0f;
	paddle.height = 0.37f;
	paddle.width = 0.05f;

	//paddle1 attribute definition
	paddle1.speed = 0.1f;
	paddle1.posX = 1.77f;
	paddle1.posY = 0.0f;
	paddle1.height = 0.37f;
	paddle1.width = 0.05f;

	//ball attribute definition
	ball.speed = 1.5f;
	ball.posX = 0.0f;
	ball.posY = 0.0f;
	ball.height = 0.08f;
	ball.width = 0.08f;

	//ball motion
	int rand_y = rand() % 15 - 7;
	ball.yDirection = rand_y / 11.0f;
	int rand_x = rand() % 10;
	ball.xDirection = 1.05f ? -1.05f : rand_x >= 5;

	//top and bottom bar attributes
	topBar.posX = 0.0f;
	topBar.posY = 0.99f;
	topBar.width = 4.33f;
	topBar.height = 0.05f;

	bottomBar.posX = 0.0f;
	bottomBar.posY = -0.99f;
	bottomBar.width = 4.33f;
	bottomBar.height = 0.05f;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(111.0f, 111.0f, 111.0f, 1.0f);
}

void ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			gameIsRunning = false;
		}
	}
	
	if (keys[SDL_SCANCODE_W]) {
		if (paddle.posY < 0.99f) {
			paddle.yDirection += 0.05f;
		}
	}
	else if (keys[SDL_SCANCODE_S]) {
		if (paddle.posY > -0.99f) {
			paddle.yDirection -= 0.05f;
		}
	}
	else {
		paddle.yDirection = 0.0f;
	}
	if (keys[SDL_SCANCODE_UP]) {
		if (paddle1.posY < 0.99f) {
			paddle1.yDirection += 0.05f;
		}
	}
	else if (keys[SDL_SCANCODE_DOWN]) {
		if (paddle1.posY > -0.99f) {
			paddle1.yDirection -= 0.05f;
		}
	}
	else {
		paddle1.yDirection = 0.0f;
	}
	
}


//FUnctioning for detecting Collision
bool collision(Entity &object, Entity &object1, float area) {
	float yDist = fabs(object.posY - object1.posY) - ((object.height + object1.height) / 2.0f);
	float xDist = fabs(object.posX - object1.posX) - ((object.width + object1.width) / 2.0f);
	if (xDist < area && yDist < area) {
		return true;
	}
	return false;
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;

void Update() { //Animation
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;

	//Movements
	paddle.displacement(deltaTime);
	paddle1.displacement(deltaTime);
	ball.displacement(deltaTime);

	
	// Collision tracker
	if (collision(paddle, topBar, 0.05f) || collision(paddle, bottomBar, 0.05f)) {
		paddle.displacement(-1.0f * deltaTime);
	}

	if (collision(paddle1, topBar, 0.05f) || collision(paddle1, bottomBar, 0.05f)) {
		paddle1.displacement(-1.0f * deltaTime);
	}

	if (collision(ball, topBar, 0.009f) || collision(ball, bottomBar, 0.009f)) {
		ball.yDirection = -ball.yDirection;
	}

	if (collision(ball, paddle, 0.009f) || collision(ball, paddle1, 0.009f)) {
		ball.xDirection = -ball.xDirection;
		if (collision(ball, paddle, 0.009f)) {
			float centerDist = ball.posY - paddle.posY;
			ball.yDirection = centerDist / (paddle.height / 2) * 0.8f;
		}
		else if (collision(ball, paddle1, 0.009f)) {
			float centerDist = ball.posY - paddle1.posY;
			ball.yDirection = centerDist / (paddle1.height / 2) * 0.8f;
		}
	}
	
	// Score Updates
	if(abs(ball.posX) > 2.05f) {
		if (ball.posX > paddle1.posX) {
			score += 1;
		}
		else if (ball.posX < paddle.posX) {
			score1 += 1;
		}

		//Check for ending game or continuing
		if ((score == 4 || score1 == 4)) {
			gameIsRunning = false;
		}
		else {
			reset(paddle, paddle1, ball);
		}
	}

	
}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	paddle.Render(program);
	paddle1.Render(program);
	ball.Render(program);
	topBar.Render(program);
	bottomBar.Render(program);

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
