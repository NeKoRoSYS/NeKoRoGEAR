#include "Application.h"
#include "Engine/Rendering/Window.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Events/Events.h"
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <iostream>

Application::Application() : window("NeKoRoGINE", 1280, 720), renderer(window) { 
    debugMenu.Init(window.window, window.glContext);
}

Application::~Application() {
    debugMenu.Shutdown();
}

void Application::Run() {
    isRunning = true;
    Uint64 lastTime = SDL_GetPerformanceCounter();

    while (isRunning) {
        Uint64 currentTime = SDL_GetPerformanceCounter();
        float deltaTime = (float)((currentTime - lastTime) / (double)SDL_GetPerformanceFrequency());
        lastTime = currentTime;
        HandleEvents();

        for (const auto& event : eventBus.GetEvents()) {
            std::visit([this](auto&& e) {
                using T = std::decay_t<decltype(e)>;
                if constexpr (std::is_same_v<T, WindowCloseEvent>) {
                    isRunning = false;
                } 
                else if constexpr (std::is_same_v<T, WindowResizeEvent>) {
                    std::cout << "Window resized to: " << e.width << "x" << e.height << "\n";
                }
            }, event);
        }
        eventBus.Clear();
        renderer.Clear();
        debugMenu.Begin();
        debugMenu.Draw(renderer.clearColor);
        debugMenu.End();
        debugMenu.RenderDrawData();
        window.Swap();
    }
}

void Application::HandleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        debugMenu.pointerCaptured = false; 
        
        debugMenu.ProcessEvent(&event);
        
        if (event.type == SDL_EVENT_QUIT) {
            eventBus.Push(WindowCloseEvent{});
            continue; 
        }

        if (debugMenu.pointerCaptured) continue;
                 
        switch (event.type) {
            case SDL_EVENT_WINDOW_RESIZED:
                eventBus.Push(WindowResizeEvent{
                    event.window.data1, 
                    event.window.data2
                });
                break;
                             
            case SDL_EVENT_KEY_DOWN:
                eventBus.Push(KeyPressEvent{
                    event.key.key, 
                    event.key.repeat != 0
                });
                break;
        }
    }
}