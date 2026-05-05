#pragma once

#include "Engine/ECS/ECS.h"
#include "Engine/Events/EventBus.h"
#include "Engine/Rendering/Window.h"
#include "Engine/Rendering/Renderer.h"
#include "DebugMenu.h"

class Application {
    public:
        Application();
        ~Application();
        void Run();
    private:
        Registry registry;
        EventBus eventBus;
        Window window;
        Renderer renderer;
        DebugMenu debugMenu;
        bool isRunning;
        uint32_t windowWidth = 1280;
        uint32_t windowHeight = 720;
        void HandleEvents();
};