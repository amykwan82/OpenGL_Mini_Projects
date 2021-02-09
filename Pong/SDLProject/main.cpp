#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool startGame = false;
bool gameOver = false;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix, ball_Matrix, paddleA_Matrix, paddleB_Matrix;


glm::vec3 ball_position = glm::vec3(0,0,0);
glm::vec3 paddleA_position = glm::vec3(-4.75,0,0);
glm::vec3 paddleB_position = glm::vec3(4.75,0,0);

glm:: vec3 paddleA_movement = glm::vec3(0,0,0);
glm:: vec3 paddleB_movement = glm::vec3(0,0,0);

float ball_moveX = 2.00f;
float ball_moveY = 1.40f;

float ball_speed = 1.0f;
float paddleA_speed = 1.0f;
float paddleB_speed = 1.0f;

Mix_Music *music;
Mix_Chunk *bounce;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Pong Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow); //making sure you're editing this specific window
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480); //screen size
    
    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("dooblydoo.mp3");
    Mix_PlayMusic(music,-1); //endlyless loop
    Mix_VolumeMusic(MIX_MAX_VOLUME/4);
    
    bounce = Mix_LoadWAV("bounce.wav");
    
    viewMatrix = glm::mat4(1.0f);
    ball_Matrix = glm::mat4(1.0f);
    paddleA_Matrix = glm::mat4(1.0f);
    paddleB_Matrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f); //the view
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    program.SetColor(1.0f, 0.8f, 0.3f, 0.5f);
    
    glUseProgram(program.programID);
    
    glClearColor(0.9f, 0.2f, 1.0f, 0.7f);
}

bool isColliding(glm::vec3 objectA_position, glm::vec3 objectB_position) {
    float xdist = fabs(objectA_position.x - objectB_position.x) - ((0.25+0.25)/2.0f);
    float ydist = fabs(objectA_position.y - objectB_position.y) - ((0.25+1.00)/2.0f);
    
    if (xdist < 0 && ydist < 0) {
        return true;
    }
    
    return false;
}

void ProcessInput() {
    
    paddleA_movement = glm::vec3(0); //if nothing is pressed dont go anywhere
    paddleB_movement = glm::vec3(0); //if nothing is pressed dont go anywhere

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        startGame = true;
                        break;
                }
                break; //SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (gameOver == false) {
        if (keys[SDL_SCANCODE_W]) {
            if (paddleA_position.y < 2.75f) {
                paddleA_movement.y = 3.0f;
            }
        }
        else if (keys[SDL_SCANCODE_S]) {
            if (paddleA_position.y > -2.75f) {
                paddleA_movement.y = -3.0f;
            }
        }
        else if (keys[SDL_SCANCODE_UP]) {
            if (paddleB_position.y < 2.75f) {
                paddleB_movement.y = 3.0f;
            }
        }
        else if (keys[SDL_SCANCODE_DOWN]) {
            if (paddleB_position.y > -2.75f) {
                paddleB_movement.y = -3.0f;
            }
        }
        
        if (glm::length(paddleA_movement) > 3.0f) {
            paddleA_movement = glm::normalize(paddleA_movement);
        }
        if (glm::length(paddleB_movement) > 3.0f) {
            paddleB_movement = glm::normalize(paddleB_movement);
        }
    }
    
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    // Add (direction * units per second * elapsed time)
    paddleA_position += paddleA_movement * paddleA_speed * deltaTime;
    paddleB_position += paddleB_movement * paddleB_speed * deltaTime;
    
    paddleA_Matrix = glm::mat4(1.0f);
    paddleA_Matrix = glm::translate(paddleA_Matrix, paddleA_position);
    
    paddleB_Matrix = glm::mat4(1.0f);
    paddleB_Matrix = glm::translate(paddleB_Matrix, paddleB_position);
    
    if (startGame && ball_position.x > -4.8f && ball_position.x < 4.8f && ball_position.y > -3.6f && ball_position.y < 3.6f) {
        ball_position.x += ball_moveX * ball_speed * deltaTime;
        ball_position.y += ball_moveY * ball_speed * deltaTime;
        
        ball_Matrix = glm::mat4(1.0f);
        ball_Matrix = glm::translate(ball_Matrix, ball_position);
        
        if (isColliding(ball_position,paddleB_position)) {
            ball_moveX *= -1;
            Mix_PlayChannel(-1,bounce,0);
        }
        
        if (isColliding(ball_position,paddleA_position)) {
            ball_moveX *= -1;
            Mix_PlayChannel(-1,bounce,0);
        }
        
        if (ball_position.y > 3.5f) {
            ball_moveY *= -1;
        }
        if (ball_position.y < -3.5f) {
            ball_moveY *= -1;
        }
    
    }
    
    if (ball_position.x < -4.75f || ball_position.x > 4.75f) {
        gameOver = true;
    }
    
    
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    float verticesBall[] = { -0.25f, -0.25f, 0.25f, -0.25f, 0.25f, 0.25f, -0.25f, -0.25f, 0.25f, 0.25f, -0.25f, 0.25f }; // vertices of the  ball
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesBall);
    program.SetModelMatrix(ball_Matrix);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    
    float verticesA[] = { -0.25f, -1.00f, 0.25f, -1.00f, 0.25f, 1.00f, -0.25f, -1.00f, 0.25f, 1.00f, -0.25f, 1.00f }; // vertices of the paddle A
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesA);
    program.SetModelMatrix(paddleA_Matrix);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    
    float verticesB[] = { -0.25f, -1.00f, 0.25f, -1.00f, 0.25f, 1.00f, -0.25f, -1.00f, 0.25f, 1.00f, -0.25f, 1.00f };  // vertices of the paddle B
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesB);
    program.SetModelMatrix(paddleB_Matrix);
    glEnableVertexAttribArray(program.positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    
    
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
