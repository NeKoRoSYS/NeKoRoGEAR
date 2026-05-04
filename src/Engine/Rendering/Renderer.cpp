#include "Renderer.h"
#include "Engine/Components/DebugMenu.h"
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <iostream>

Renderer::Renderer() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Failed to initialize SDL3: " << SDL_GetError() << std::endl;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow(
        "NeKoRoGINE", 
        1280, 720, SDL_WINDOW_OPENGL
    );

    if (!window) {
        SDL_Quit();
        throw std::runtime_error(std::string("Failed to create window: ") + SDL_GetError());
    }

    glContext = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    if (!glContext) {
        std::cerr << "Failed to create GL context: " << SDL_GetError() << std::endl;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
    }
}

Renderer::~Renderer() {
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Renderer::Clear() {
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Swap() {
    SDL_GL_SwapWindow(window);
}