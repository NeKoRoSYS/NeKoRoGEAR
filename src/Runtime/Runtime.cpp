#include "Core/EntryPoint.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Assets/ModelLoader.h"
#include "Core/Components/Application.h"
#include "Core/Components/Scene.h"
#include "Core/Components/SceneSerializer.h"
#include "Core/ECS/Components/Transform.h"
#include "Core/ECS/Components/Camera.h"
#include "Core/ECS/Components/Renderable.h"
#include "Core/ECS/Components/Tag.h"
#include "Core/ECS/Systems/RenderSystem.h"
#include "Core/ECS/Systems/TransformSystem.h"
#include "Core/ECS/Systems/CameraSystem.h"
#include "Core/ECS/Systems/CameraControllerSystem.h"

class RuntimeApp : public Application {
private:
    std::shared_ptr<RenderSystem> renderSystem;
    std::shared_ptr<TransformSystem> transformSystem;
    std::shared_ptr<CameraSystem> cameraSystem;
    std::shared_ptr<CameraControllerSystem> cameraControllerSystem;
    
    std::shared_ptr<Scene> activeScene;
    int windowWidth = 1280;
    int windowHeight = 720;

    void SetupScene(std::shared_ptr<Scene> scene) {
        auto& registry = scene->GetRegistry();

        registry.RegisterComponent<TagComponent>();
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

        std::unique_ptr<Model> myModel = ModelLoader::Load("../../assets/models/Miku.fbx");
        AssetManager::Get().models.Add(std::move(myModel));

        std::unique_ptr<Shader> defaultShader = std::make_unique<Shader>("../../assets/shaders/basic.vert", "../../assets/shaders/basic.frag");
        AssetManager::Get().shaders.Add(std::move(defaultShader));

        AssetManager::Get().textures.Add(Texture::Load("../../assets/models/Miku.png"));

        SceneSerializer serializer(activeScene);
        if (!serializer.Deserialize("../../assets/scenes/dev_map.json")) {
            std::cerr << "FATAL: Failed to load startup scene (dev_map.json)." << std::endl;
            
            Entity fallbackCamera = activeScene->CreateEntity("Fallback Camera");
            registry.AddComponent(fallbackCamera, CameraComponent{});
            auto& camTransform = registry.GetComponent<TransformComponent>(fallbackCamera);
            camTransform.position = glm::vec3(0.0f, 0.0f, 5.0f);
        }
    }

    void BindInputs() {
        input.BindAction(InputAction::ToggleCameraMode, MouseButton::Right);
        input.BindAxis(InputAxis::LookRight, MouseAxis::X,  1.0f);
        input.BindAxis(InputAxis::LookUp,    MouseAxis::Y, -1.0f);
    }

public:
    RuntimeApp() : Application() {}

    void OnInit() override {
        activeScene = std::make_shared<Scene>();
        SetupScene(activeScene);
        BindInputs();
    }

    void OnUpdate(float deltaTime) override {
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

        bool cameraModeActive = input.IsActionActive(InputAction::ToggleCameraMode);
        window.SetCursorEnabled(!cameraModeActive);

        auto& registry = activeScene->GetRegistry();
        cameraControllerSystem->Update(registry, deltaTime, cameraModeActive);
        transformSystem->Update(registry);

        float aspectRatio = (float)windowWidth / (float)(windowHeight > 0 ? windowHeight : 1);
        cameraSystem->Update(registry, aspectRatio);
    }

    void OnRender() override {
        renderer.Clear();
        renderSystem->Update(activeScene->GetRegistry());
    }
};

Application* CreateApplication() {
    return new RuntimeApp();
}