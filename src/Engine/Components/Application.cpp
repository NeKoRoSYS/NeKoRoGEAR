#include <imgui.h>
#include "Application.h"
#include "Engine/Assets/ModelLoader.h"
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

#include "Engine/Rendering/Buffer.h"
#include "Engine/Rendering/Shader.h"
#include <vector>

void Application::Run() {
    isRunning = true;
    Registry registry;

    registry.RegisterComponent<TransformComponent>();
    registry.RegisterComponent<CameraComponent>();
    registry.RegisterComponent<RenderComponent>();

    auto renderSystem = registry.RegisterSystem<RenderSystem>();
    auto transformSystem = registry.RegisterSystem<TransformSystem>();
    auto cameraSystem = registry.RegisterSystem<CameraSystem>();
    auto cameraControllerSystem = registry.RegisterSystem<CameraControllerSystem>();

    Signature renderSignature;
    renderSignature.set(GetComponentTypeID<RenderComponent>());
    renderSignature.set(GetComponentTypeID<TransformComponent>());
    registry.SetSystemSignature<RenderSystem>(renderSignature);

    Signature transformSignature;
    transformSignature.set(GetComponentTypeID<TransformComponent>());
    registry.SetSystemSignature<TransformSystem>(transformSignature);

    Signature cameraSignature;
    cameraSignature.set(GetComponentTypeID<CameraComponent>());
    cameraSignature.set(GetComponentTypeID<TransformComponent>());
    registry.SetSystemSignature<CameraSystem>(cameraSignature);
    registry.SetSystemSignature<CameraControllerSystem>(cameraSignature);

    Entity mainCamera = registry.CreateEntity();
    registry.AddComponent(mainCamera, TransformComponent{
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, -90.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    });
    registry.AddComponent(mainCamera, CameraComponent{});

    std::unique_ptr<Model> myModel = ModelLoader::Load("../../assets/models/Miku.fbx");
    AssetHandle modelHandle = AssetManager::Get().models.Add(std::move(myModel));

    std::unique_ptr<Shader> defaultShader = std::make_unique<Shader>("../../assets/shaders/basic.vert", "../../assets/shaders/basic.frag");
    AssetHandle shaderHandle = AssetManager::Get().shaders.Add(std::move(defaultShader));

    AssetHandle modelTex = AssetManager::Get().textures.Add(Texture::Load("../../assets/models/Miku.png"));

    Entity modelEntity = registry.CreateEntity();
    registry.AddComponent(modelEntity, TransformComponent{
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    });

    registry.AddComponent(modelEntity, RenderComponent{ 
        modelHandle, 
        shaderHandle, 
        {modelTex}
    });

    Uint64 lastTime = SDL_GetPerformanceCounter();

    while (isRunning) {
        Uint64 currentTime = SDL_GetPerformanceCounter();
        float deltaTime = (float)((currentTime - lastTime) / (double)SDL_GetPerformanceFrequency());
        lastTime = currentTime;
        HandleEvents();
        
        for (const auto& event : eventBus.GetEvents()) {
            std::visit([this, &cameraControllerSystem](auto&& e) {
                using T = std::decay_t<decltype(e)>;
                if constexpr (std::is_same_v<T, WindowCloseEvent>) {
                    isRunning = false;
                } else if constexpr (std::is_same_v<T, WindowResizeEvent>) {
                    std::cout << "Window resized to: " << e.width << "x" << e.height << "\n";
                    windowWidth = e.width;
                    windowHeight = e.height;
                    RenderCommand::SetViewport(0, 0, windowWidth, windowHeight);
                } else if constexpr (std::is_same_v<T, MouseButtonEvent>) {
                    if (e.button == SDL_BUTTON_RIGHT && e.pressed) {
                        cameraControllerSystem->ToggleCameraMode(window.GetNativeWindow());
                    }
                }
            }, event);
        }

        eventBus.Clear();

        cameraControllerSystem->Update(registry, deltaTime);
        transformSystem->Update(registry);
        float aspectRatio = (float)windowWidth / (float)(windowHeight > 0 ? windowHeight : 1);
        cameraSystem->Update(registry, aspectRatio);

        renderer.Clear();

        auto& modelTransform = registry.GetComponent<TransformComponent>(modelEntity);
        if (debugMenu.spinObject) {
            modelTransform.rotation.x += 45.0f * deltaTime;
            modelTransform.rotation.y += 45.0f * deltaTime;
        }

        renderSystem->Update(registry);

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
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                eventBus.Push(MouseButtonEvent{ event.button.button, true });
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                eventBus.Push(MouseButtonEvent{ event.button.button, false });
                break;
        }
    }
}