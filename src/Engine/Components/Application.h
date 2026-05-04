#pragma once

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
        EventBus eventBus;
        Window window;
        Renderer renderer;
        DebugMenu debugMenu;
        bool isRunning;
        void HandleEvents();
};