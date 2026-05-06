#include "Core/EntryPoint.h"
#include "Core/Components/Application.h"
#include "Core/Components/InputManager.h"
#include "Core/ECS/Components/Transform.h"
#include "Core/ECS/Components/Camera.h"
#include "Core/ECS/Components/Renderable.h"
#include "Core/ECS/Systems/RenderSystem.h"
#include "Core/ECS/Systems/TransformSystem.h"
#include "Core/ECS/Systems/CameraSystem.h"
#include "Core/ECS/Systems/CameraControllerSystem.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Assets/ModelLoader.h"
#include "Core/Rendering/RenderCommand.h"
#include <iostream>

class RuntimeApp : public Application {
private:
    std::shared_ptr<RenderSystem> renderSystem;
    std::shared_ptr<TransformSystem> transformSystem;
    std::shared_ptr<CameraSystem> cameraSystem;
    std::shared_ptr<CameraControllerSystem> cameraControllerSystem;
    
    Entity mainCamera;
    Entity modelEntity;

    int windowWidth = 1280;
    int windowHeight = 720;

public:
    RuntimeApp() : Application() {}

    void OnInit() override {
        registry.RegisterComponent<TransformComponent>();
        registry.RegisterComponent<CameraComponent>();
        registry.RegisterComponent<RenderComponent>();

        renderSystem = registry.RegisterSystem<RenderSystem>();
        transformSystem = registry.RegisterSystem<TransformSystem>();
        cameraSystem = registry.RegisterSystem<CameraSystem>();
        cameraControllerSystem = registry.RegisterSystem<CameraControllerSystem>();

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

        InputManager& input = InputManager::Get();
        input.BindAction(InputAction::ToggleCameraMode, MouseButton::Right);
        input.BindAction(InputAction::Sprint, KeyCode::Shift);
        input.BindAxis(InputAxis::MoveForward, KeyCode::W,  1.0f);
        input.BindAxis(InputAxis::MoveForward, KeyCode::S, -1.0f);
        input.BindAxis(InputAxis::MoveRight,   KeyCode::D,  1.0f);
        input.BindAxis(InputAxis::MoveRight,   KeyCode::A, -1.0f);
        input.BindAxis(InputAxis::MoveUp,      KeyCode::E,  1.0f);
        input.BindAxis(InputAxis::MoveUp,      KeyCode::Q, -1.0f);
        input.BindAxis(InputAxis::LookRight, MouseAxis::X,  1.0f);
        input.BindAxis(InputAxis::LookUp,    MouseAxis::Y, -1.0f);

        mainCamera = registry.CreateEntity();
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

        modelEntity = registry.CreateEntity();
        registry.AddComponent(modelEntity, TransformComponent{
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f)
        });
        registry.AddComponent(modelEntity, RenderComponent{ modelHandle, shaderHandle, {modelTex} });
    }

    void OnUpdate(float deltaTime) override {
        InputManager& input = InputManager::Get();
        input.Update();

        for (const auto& event : eventBus.GetEvents()) {
            input.ProcessRawEvent(event, eventBus);
            
            std::visit([this](auto&& e) {
                using T = std::decay_t<decltype(e)>;
                if constexpr (std::is_same_v<T, WindowCloseEvent>) {
                    isRunning = false;
                } else if constexpr (std::is_same_v<T, WindowResizeEvent>) {
                    windowWidth = e.width;
                    windowHeight = e.height;
                    RenderCommand::SetViewport(0, 0, windowWidth, windowHeight);
                }
            }, event);
        }
        eventBus.Clear();

        if (input.IsActionActive(InputAction::ToggleCameraMode)) {
            cameraControllerSystem->SetCameraMode(window.GetNativeWindow(), true);
        } else {
            cameraControllerSystem->SetCameraMode(window.GetNativeWindow(), false);
        }

        cameraControllerSystem->Update(registry, deltaTime);
        transformSystem->Update(registry);

        float aspectRatio = (float)windowWidth / (float)(windowHeight > 0 ? windowHeight : 1);
        cameraSystem->Update(registry, aspectRatio);
    }

    void OnRender() override {
        renderer.Clear();
        renderSystem->Update(registry);
    }
};

Application* CreateApplication() {
    return new RuntimeApp();
}