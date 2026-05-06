#pragma once

#include <SDL3/SDL.h>

class Window {
    public:
        SDL_Window* window;
        SDL_GLContext glContext;
        Window(const char* title, int width, int height);
        ~Window();

        void* GetProcAddress() { return (void*)SDL_GL_GetProcAddress; }
        SDL_Window* GetNativeWindow() { return window; }
        void Swap() { SDL_GL_SwapWindow(window); }
};