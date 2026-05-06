#include <imgui.h>
#include "Application.h"
#include "Core/Assets/ModelLoader.h"
#include "Core/Rendering/Window.h"
#include "Core/Rendering/Renderer.h"
#include "Core/Events/Events.h"
#include "Core/ECS/ECS.h"
#include "Core/ECS/Components/Transform.h"
#include "Core/ECS/Components/Camera.h"
#include "Core/ECS/Systems/TransformSystem.h"
#include "Core/ECS/Systems/CameraSystem.h"
#include "Core/ECS/Systems/CameraControllerSystem.h"
#include "Core/ECS/Systems/RenderSystem.h"
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <iostream>

Application::Application() : window("NeKoRoGEAR", 1280, 720), renderer(window) {
    debugMenu.Init(window.GetNativeWindow(), window.glContext);
}

Application::~Application() {
    debugMenu.Shutdown();
}

#include "Core/Rendering/Buffer.h"
#include "Core/Rendering/Shader.h"
#include <vector>

void Application::Run() {
    OnInit();
    Uint64 lastTime = SDL_GetPerformanceCounter();
    
    while (isRunning) {
        Uint64 currentTime = SDL_GetPerformanceCounter();
        float deltaTime = (float)((currentTime - lastTime) / (double)SDL_GetPerformanceFrequency());
        lastTime = currentTime;
        
        HandleEvents();

        debugMenu.Begin();
        OnImGuiRender();
        debugMenu.End();
        OnUpdate(deltaTime);
        OnRender();
        debugMenu.RenderDrawData();
        window.Swap();
    }
    
    OnShutdown();
}

void Application::HandleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
       debugMenu.pointerCaptured = false;  
        debugMenu.ProcessEvent(&event);
        
        bool mouseCaptured = ImGui::GetIO().WantCaptureMouse;
        bool keyboardCaptured = ImGui::GetIO().WantCaptureKeyboard;

        if (mouseCaptured && (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || 
                              event.type == SDL_EVENT_MOUSE_BUTTON_UP || 
                              event.type == SDL_EVENT_MOUSE_MOTION)) {
            continue;
        }
        if (keyboardCaptured && (event.type == SDL_EVENT_KEY_DOWN || 
                                 event.type == SDL_EVENT_KEY_UP)) {
            continue;
        }
        
        switch (event.type) {
            case SDL_EVENT_WINDOW_RESIZED:
                eventBus.Push(WindowResizeEvent{ event.window.data1, event.window.data2 });
                break;
            case SDL_EVENT_KEY_DOWN:
                eventBus.Push(KeyPressEvent{ event.key.key, true, event.key.repeat != 0 });
                break;
            case SDL_EVENT_KEY_UP:
                eventBus.Push(KeyPressEvent{ event.key.key, false, false });
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                eventBus.Push(MouseButtonEvent{ event.button.button, true });
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                eventBus.Push(MouseButtonEvent{ event.button.button, false });
                break;
            case SDL_EVENT_MOUSE_MOTION:
                eventBus.Push(MouseMotionEvent{ event.motion.xrel, event.motion.yrel });
                break;
        }
    }
}
