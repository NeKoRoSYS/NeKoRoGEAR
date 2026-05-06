#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h> 

class DebugMenu {
    public:
        bool pointerCaptured;
        bool spinObject;
        void Init(SDL_Window* window, SDL_GLContext glContext);
        void Shutdown();
        void Begin();
        void End();
        void Draw(float* clearColor); 
        void ProcessEvent(SDL_Event* event);
        void RenderDrawData();
};