#include <imgui.h>
#include "Application.h"
#include "Engine/Rendering/Window.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Events/Events.h"
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Components/Transform.h"
#include "Engine/ECS/Components/Camera.h"
#include "Engine/ECS/Systems/TransformSystem.h"
#include "Engine/ECS/Systems/CameraSystem.h"
#include "Engine/ECS/Systems/CameraControllerSystem.h"
#include "Engine/ECS/Systems/RenderSystem.h"
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <iostream>

Application::Application() : window("NeKoRoGINE", 1280, 720), renderer(window) { 
    debugMenu.Init(window.window, window.glContext);
}

Application::~Application() {
    debugMenu.Shutdown();
}

#include "Engine/Rendering/Mesh.h"
#include "Engine/Rendering/Shader.h"
#include <vector>

Mesh* CreateTestCube() {
    std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,
        1, 5, 6, 6, 2, 1,
        7, 6, 5, 5, 4, 7,
        4, 0, 3, 3, 7, 4,
        4, 5, 1, 1, 0, 4,
        3, 2, 6, 6, 7, 3 
    };

    return new Mesh(vertices, indices); 
}

void Application::Run() {
    isRunning = true;

    Registry registry;
    registry.RegisterComponent<TransformComponent>();
    registry.RegisterComponent<CameraComponent>();

    registry.RegisterComponent<RenderComponent>();

    Entity mainCamera = registry.CreateEntity();
    registry.AddComponent(mainCamera, TransformComponent{
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, -90.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    });
    registry.AddComponent(mainCamera, CameraComponent{});

    Mesh* cubeMesh = CreateTestCube();
    Shader* defaultShader = new Shader("../../assets/shaders/basic.vert", "../../assets/shaders/basic.frag");

    Entity cube = registry.CreateEntity();
    registry.AddComponent(cube, TransformComponent{
        glm::vec3(0.0f, 0.0f, 0.0f), // At origin
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    });
    registry.AddComponent(cube, RenderComponent{ cubeMesh, defaultShader });

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
        CameraControllerSystem::Update(registry, window.GetNativeWindow(), deltaTime);
        TransformSystem::Update(registry);
        CameraSystem::Update(registry, 1280.0f / 720.0f);
        renderer.Clear();

        auto& cubeTransform = registry.GetComponent<TransformComponent>(cube);
        cubeTransform.rotation.x += 45.0f * deltaTime;
        cubeTransform.rotation.y += 45.0f * deltaTime;

        RenderSystem::Update(registry);

        debugMenu.Begin();
        
        ImGui::Begin("Camera Debug");
        auto& camTransform = registry.GetComponent<TransformComponent>(mainCamera);
        ImGui::Text("Position: X:%.2f Y:%.2f Z:%.2f", camTransform.position.x, camTransform.position.y, camTransform.position.z);
        ImGui::Text("Pitch: %.2f Yaw: %.2f", camTransform.rotation.x, camTransform.rotation.y);
        ImGui::Text("Hold Right-Click to move.");
        ImGui::End();

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