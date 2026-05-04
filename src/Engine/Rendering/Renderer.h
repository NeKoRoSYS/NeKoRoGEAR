#pragma once

struct SDL_Window;
#include "Engine/Components/DebugMenu.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h> 

class Renderer {
    public:
        SDL_Window* window;
        SDL_GLContext glContext;
        float clearColor[4] = { 0.8f, 0.2f, 0.3f, 1.0f };
        Renderer();
        ~Renderer();
        void Clear();
        void Swap();
};