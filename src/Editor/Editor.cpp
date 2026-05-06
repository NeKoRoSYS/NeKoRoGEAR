#include "Core/EntryPoint.h"
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

        mainCamera = activeScene->CreateEntity("Main Camera");
        auto& camTransform = registry.GetComponent<TransformComponent>(mainCamera);
        camTransform.position = glm::vec3(0.0f, 0.0f, 3.0f);
        camTransform.rotation = glm::vec3(0.0f, -90.0f, 0.0f);
        registry.AddComponent(mainCamera, CameraComponent{});

        std::unique_ptr<Model> myModel = ModelLoader::Load("../../assets/models/Miku.fbx");
        AssetHandle modelHandle = AssetManager::Get().models.Add(std::move(myModel));
        
        std::unique_ptr<Shader> defaultShader = std::make_unique<Shader>("../../assets/shaders/basic.vert", "../../assets/shaders/basic.frag");
        AssetHandle shaderHandle = AssetManager::Get().shaders.Add(std::move(defaultShader));
        
        AssetHandle manualTex = AssetManager::Get().textures.Add(Texture::Load("../../assets/models/Miku.png"));
        std::vector<AssetHandle> overrides;
        for (size_t i = 0; i < AssetManager::Get().models.Get(modelHandle)->meshHandles.size(); ++i) {
            overrides.push_back(manualTex); 
        }

        modelEntity = activeScene->CreateEntity("Miku Model");
        registry.AddComponent(modelEntity, RenderComponent{ modelHandle, shaderHandle, overrides });
    }

    void BindInputs() {
        input.BindAction(InputAction::ToggleCameraMode, MouseButton::Right);
        input.BindAction(InputAction::Sprint, KeyCode::LShift);
        input.BindAxis(InputAxis::MoveForward, KeyCode::W,  1.0f);
        input.BindAxis(InputAxis::MoveForward, KeyCode::S, -1.0f);
        input.BindAxis(InputAxis::MoveRight,   KeyCode::D,  1.0f);
        input.BindAxis(InputAxis::MoveRight,   KeyCode::A, -1.0f);
        input.BindAxis(InputAxis::MoveUp,      KeyCode::E,  1.0f);
        input.BindAxis(InputAxis::MoveUp,      KeyCode::Q, -1.0f);
        input.BindAxis(InputAxis::LookRight, MouseAxis::X,  1.0f);
        input.BindAxis(InputAxis::LookUp,    MouseAxis::Y, -1.0f);
    }

public:
    EditorApp() : Application() {}

    std::shared_ptr<Scene> activeScene;
    Entity selectedEntity = -1;

    void OnInit() override {
        activeScene = std::make_shared<Scene>();
        SetupScene(activeScene);
        BindInputs();
    }

    void OnUpdate(float deltaTime) override {
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

        bool cameraModeActive = input.IsActionActive(InputAction::ToggleCameraMode);
        window.SetCursorEnabled(!cameraModeActive);
        auto& registry = activeScene->GetRegistry();
        cameraControllerSystem->Update(registry, deltaTime, cameraModeActive);
        auto& modelTransform = registry.GetComponent<TransformComponent>(modelEntity);
        
        if (registry.HasComponent<TransformComponent>(modelEntity)) {
            auto& modelTransform = registry.GetComponent<TransformComponent>(modelEntity);
            if (debugMenu.spinObject) {
                modelTransform.rotation.x += 45.0f * deltaTime;
                modelTransform.rotation.y += 45.0f * deltaTime;
            }
        }

        transformSystem->Update(registry);
        cameraSystem->Update(registry, 1280.0f / 720.0f);
    }

    void OnRender() override {
        renderer.Clear();
        renderSystem->Update(activeScene->GetRegistry());
    }

    void OnImGuiRender() override {
        auto& registry = activeScene->GetRegistry();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Scene")) {
                    activeScene = std::make_shared<Scene>();
                    SetupScene(activeScene);
                    selectedEntity = (Entity)-1;
                    modelEntity = (Entity)-1;
                    mainCamera = (Entity)-1;
                }
                
                if (ImGui::MenuItem("Save Scene")) {
                    SceneSerializer serializer(activeScene);
                    serializer.Serialize("../../assets/scenes/dev_map.json");
                }
                
                if (ImGui::MenuItem("Load Scene")) {
                    auto newScene = std::make_shared<Scene>();
                    SetupScene(newScene);
                    
                    SceneSerializer serializer(newScene);
                    if (serializer.Deserialize("../../assets/scenes/dev_map.json")) {
                        activeScene = newScene;
                        selectedEntity = (Entity)-1;
                        modelEntity = (Entity)-1;
                        mainCamera = (Entity)-1;
                    }
                }
                
                if (ImGui::MenuItem("Exit")) {
                    isRunning = false;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::Begin("Camera Debug");
        if (registry.HasComponent<TransformComponent>(mainCamera)) {
            auto& camTransform = registry.GetComponent<TransformComponent>(mainCamera);
            ImGui::Text("Position: X:%.2f Y:%.2f Z:%.2f", camTransform.position.x, camTransform.position.y, camTransform.position.z);
            ImGui::Text("Pitch: %.2f Yaw: %.2f", camTransform.rotation.x, camTransform.rotation.y);
            ImGui::Text("Hold Right-Click to move.");
        }
        ImGui::End();

        ImGui::Begin("Scene Hierarchy");
        if (ImGui::Button("Create Empty Entity")) {
            activeScene->CreateEntity("Empty Entity");
        }
        ImGui::Separator();

        for (Entity entity : activeScene->GetEntities()) {
            std::string tag = "Unknown Entity";
            if (registry.HasComponent<TagComponent>(entity)) {
                tag = registry.GetComponent<TagComponent>(entity).tag;
            }
            
            ImGuiTreeNodeFlags flags = ((selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
            bool opened = ImGui::TreeNodeEx((void*)(uint64_t)entity, flags, "%s", tag.c_str());
            
            if (ImGui::IsItemClicked()) {
                selectedEntity = entity;
            }
            
            if (opened) { ImGui::TreePop(); }
        }
        ImGui::End();

        ImGui::Begin("Inspector");
        if (selectedEntity != (Entity)-1) {
            
            if (registry.HasComponent<TagComponent>(selectedEntity)) {
                auto& tagComp = registry.GetComponent<TagComponent>(selectedEntity);
                char buffer[256];
                memset(buffer, 0, sizeof(buffer));
                strncpy(buffer, tagComp.tag.c_str(), sizeof(buffer) - 1);
                if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {
                    tagComp.tag = std::string(buffer);
                }
            }

            if (registry.HasComponent<TransformComponent>(selectedEntity)) {
                if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                    auto& transform = registry.GetComponent<TransformComponent>(selectedEntity);
                    ImGui::DragFloat3("Position", &transform.position.x, 0.1f);
                    ImGui::DragFloat3("Rotation", &transform.rotation.x, 0.1f);
                    ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);
                }
            }

            if (registry.HasComponent<CameraComponent>(selectedEntity)) {
                if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
                    auto& camera = registry.GetComponent<CameraComponent>(selectedEntity);
                    ImGui::DragFloat("FOV", &camera.fov, 1.0f);
                    ImGui::DragFloat("Near Clip", &camera.nearClip, 0.1f);
                    ImGui::DragFloat("Far Clip", &camera.farClip, 1.0f);
                    ImGui::Checkbox("Is Primary", &camera.isPrimary);
                }
            }

            if (registry.HasComponent<RenderComponent>(selectedEntity)) {
                if (ImGui::CollapsingHeader("Renderable", ImGuiTreeNodeFlags_DefaultOpen)) {
                    auto& renderable = registry.GetComponent<RenderComponent>(selectedEntity);
                    // displaying the raw uint32_t AssetHandles for now
                    ImGui::Text("Model Handle: %u", renderable.modelHandle);
                    ImGui::Text("Shader Handle: %u", renderable.shaderHandle);
                }
            }

            ImGui::Separator();
            if (ImGui::Button("Add Component")) {
                ImGui::OpenPopup("AddComponentPopup");
            }
            if (ImGui::BeginPopup("AddComponentPopup")) {
                if (!registry.HasComponent<CameraComponent>(selectedEntity) && ImGui::MenuItem("Camera Component")) {
                    registry.AddComponent(selectedEntity, CameraComponent{});
                    ImGui::CloseCurrentPopup();
                }
                if (!registry.HasComponent<RenderComponent>(selectedEntity) && ImGui::MenuItem("Render Component")) {
                    registry.AddComponent(selectedEntity, RenderComponent{});
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        } else {
            ImGui::Text("Select an entity in the hierarchy to inspect.");
        }
        ImGui::End();

        debugMenu.Draw(renderer.clearColor);
    }
};

Application* CreateApplication() {
    return new EditorApp();
}