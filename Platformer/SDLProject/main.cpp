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

#include "Util.h"
#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "Level0.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

Scene *currentScene;
Scene *sceneList[4];

int numOfLives = 3;

void SwitchToScene(Scene *scene) {
    currentScene = scene;
    currentScene->Initialize();
}

Mix_Music *music;
Mix_Chunk *jump_sound;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("MONSTER DASH!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("dooblydoo.mp3");
    Mix_PlayMusic(music,-1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    
    jump_sound = Mix_LoadWAV("bounce.wav");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.7f, 0.6f, 0.8f, 1.0f);
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    sceneList[0] = new Level0();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    SwitchToScene(sceneList[0]);
    
    
}

void ProcessInput() {
    
    currentScene->state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_LEFT:
                            // Move the player left
                            break;
                                
                        case SDLK_RIGHT:
                            // Move the player right
                            break;
                        case SDLK_RETURN:
                            // clicked enter
                            currentScene->state.hasStarted = true;
                            break;
                            
                        
                            case SDLK_SPACE:
                                if (!currentScene->state.hasWon && currentScene->state.numOfLives != 0 && currentScene->state.player->collidedBottom) {
                                    currentScene->state.player->jump = true;
                                    Mix_PlayChannel(-1,jump_sound,0);
                                }
                                break;
                            
                        }
                    
                    break; // SDL_KEYDOWN
                
            }
    }
        
            const Uint8 *keys = SDL_GetKeyboardState(NULL);
        
            if (!currentScene->state.hasWon && currentScene->state.numOfLives != 0) {
                if (keys[SDL_SCANCODE_LEFT]) {
                    currentScene->state.player->movement.x = -1.0f;
                    currentScene->state.player->animIndices = currentScene->state.player->animLeft;
                }
                else if (keys[SDL_SCANCODE_RIGHT]) {
                    currentScene->state.player->movement.x = 1.0f;
                    currentScene->state.player->animIndices = currentScene->state.player->animRight;
                }
            
            
                if (glm::length(currentScene->state.player->movement) > 1.0f) {
                    currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
                }
            }
    
}

#define FIXED_TIMESTEP 0.0166666f //60 frames a second
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;
    
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
        
    }
    while (deltaTime >= FIXED_TIMESTEP) {
        currentScene->Update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
        
    }
    
    accumulator = deltaTime;
    viewMatrix = glm::mat4(1.0f);
    
    if (currentScene == sceneList[3] && currentScene->state.player->position.x > 22) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-22, 3.75, 0));
    }
    else if (currentScene == sceneList[2] && currentScene->state.player->position.x > 15) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-15, 3.75, 0));
    }
    else if (currentScene == sceneList[1] && currentScene->state.player->position.x > 12) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-12, 3.75, 0));
    }
    else if (currentScene->state.player->position.x > 5) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    }
    else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetViewMatrix(viewMatrix);
    
    currentScene->Render(&program);
    
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
        
        if (currentScene->state.nextScene >= 0) {
            int savedLives = currentScene->state.numOfLives;
            SwitchToScene(sceneList[currentScene->state.nextScene]);
            currentScene->state.numOfLives = savedLives;
        }
        
        Render();
    }
    
    Shutdown();
    return 0;
}
