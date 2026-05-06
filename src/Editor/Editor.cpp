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
#include <imgui.h>

class EditorApp : public Application {
private:
    std::shared_ptr<RenderSystem> renderSystem;
    std::shared_ptr<TransformSystem> transformSystem;
    std::shared_ptr<CameraSystem> cameraSystem;
    std::shared_ptr<CameraControllerSystem> cameraControllerSystem;
    
    Entity mainCamera;
    Entity modelEntity;

public:
    EditorApp() : Application() {}

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

        for (size_t i = 0; i < eventBus.GetEvents().size(); ++i) {
            auto event = eventBus.GetEvents()[i];
            input.ProcessRawEvent(event, eventBus);
            
            std::visit([this](auto&& e) {
                using T = std::decay_t<decltype(e)>;
                if constexpr (std::is_same_v<T, WindowCloseEvent>) {
                    isRunning = false;
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
        
        auto& modelTransform = registry.GetComponent<TransformComponent>(modelEntity);
        if (debugMenu.spinObject) {
            modelTransform.rotation.x += 45.0f * deltaTime;
            modelTransform.rotation.y += 45.0f * deltaTime;
        }

        transformSystem->Update(registry);
        cameraSystem->Update(registry, 1280.0f / 720.0f);
    }

    void OnRender() override {
        renderer.Clear();
        renderSystem->Update(registry);
    }

    void OnImGuiRender() override {
        ImGui::Begin("Camera Debug");
        auto& camTransform = registry.GetComponent<TransformComponent>(mainCamera);
        ImGui::Text("Position: X:%.2f Y:%.2f Z:%.2f", camTransform.position.x, camTransform.position.y, camTransform.position.z);
        ImGui::Text("Pitch: %.2f Yaw: %.2f", camTransform.rotation.x, camTransform.rotation.y);
        ImGui::Text("Hold Right-Click to move.");
        ImGui::End();
        debugMenu.Draw(renderer.clearColor);
    }
};

Application* CreateApplication() {
    return new EditorApp();
}