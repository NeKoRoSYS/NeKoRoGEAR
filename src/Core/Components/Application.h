#pragma once

#include "Core/ECS/ECS.h"
#include "Core/Events/EventBus.h"
#include "Core/Rendering/Window.h"
#include "Core/Rendering/Renderer.h"
#include "Core/Components/DebugMenu.h"
#include "Core/Components/InputManager.h"

class Application {
    public:
        Application();
        ~Application();
        void Run();
        virtual void OnInit() {}
        virtual void OnImGuiRender() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender() {}
        virtual void OnShutdown() {}
    protected:
        Registry registry;
        EventBus eventBus;
        Window window;
        Renderer renderer;
        DebugMenu debugMenu;
        InputManager input = InputManager::Get();
        bool isRunning = true;
    private:
        void HandleEvents();
};

Application* CreateApplication();