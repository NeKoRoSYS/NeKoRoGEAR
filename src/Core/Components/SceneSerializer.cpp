#include "SceneSerializer.h"
#include "Core/ECS/Components/Tag.h"
#include "Core/ECS/Components/Transform.h"
#include "Core/ECS/Components/Camera.h"
#include "Core/ECS/Components/Renderable.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene)
    : m_Scene(scene) {}

bool SceneSerializer::Serialize(const std::string& filepath) {
    json sceneJson;
    sceneJson["Scene"] = "NeKoRoGINE_Scene";
    sceneJson["Entities"] = json::array();

    auto& registry = m_Scene->GetRegistry();

    for (Entity entity : m_Scene->GetEntities()) {
        json entityJson;
        entityJson["EntityID"] = entity;

        if (registry.HasComponent<TagComponent>(entity)) {
            auto& tag = registry.GetComponent<TagComponent>(entity);
            entityJson["TagComponent"] = { {"Tag", tag.tag} };
        }

        if (registry.HasComponent<TransformComponent>(entity)) {
            auto& transform = registry.GetComponent<TransformComponent>(entity);
            entityJson["TransformComponent"] = {
                {"Position", {transform.position.x, transform.position.y, transform.position.z}},
                {"Rotation", {transform.rotation.x, transform.rotation.y, transform.rotation.z}},
                {"Scale",    {transform.scale.x, transform.scale.y, transform.scale.z}}
            };
        }

        if (registry.HasComponent<CameraComponent>(entity)) {
            auto& camera = registry.GetComponent<CameraComponent>(entity);
            entityJson["CameraComponent"] = {
                {"FOV", camera.fov},
                {"NearClip", camera.nearClip},
                {"FarClip", camera.farClip},
                {"IsPrimary", camera.isPrimary}
            };
        }

        if (registry.HasComponent<RenderComponent>(entity)) {
            auto& renderable = registry.GetComponent<RenderComponent>(entity);
            entityJson["RenderComponent"] = {
                {"ModelHandle", renderable.modelHandle},
                {"ShaderHandle", renderable.shaderHandle}
            };
        }

        sceneJson["Entities"].push_back(entityJson);
    }

    std::ofstream file(filepath);
    if (file.is_open()) {
        file << sceneJson.dump(4);
        return true;
    }
    return false;
}

bool SceneSerializer::Deserialize(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    json sceneJson;
    try {
        file >> sceneJson;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON Parse Error in " << filepath << ": " << e.what() << std::endl;
        return false;
    }
    
    if (!sceneJson.contains("Entities")) return false;

    auto& registry = m_Scene->GetRegistry();

    for (const auto& entityJson : sceneJson["Entities"]) {
        std::string name = "Deserialized Entity";
        if (entityJson.contains("TagComponent")) {
            name = entityJson["TagComponent"]["Tag"].get<std::string>();
        }

        Entity deserializedEntity = m_Scene->CreateEntity(name);

        if (entityJson.contains("TransformComponent")) {
            auto& transform = registry.GetComponent<TransformComponent>(deserializedEntity);
            auto pos = entityJson["TransformComponent"]["Position"];
            auto rot = entityJson["TransformComponent"]["Rotation"];
            auto scale = entityJson["TransformComponent"]["Scale"];

            transform.position = glm::vec3(pos[0], pos[1], pos[2]);
            transform.rotation = glm::vec3(rot[0], rot[1], rot[2]);
            transform.scale    = glm::vec3(scale[0], scale[1], scale[2]);
        }

        if (entityJson.contains("CameraComponent")) {
            CameraComponent camera;
            camera.fov = entityJson["CameraComponent"]["FOV"];
            camera.nearClip = entityJson["CameraComponent"]["NearClip"];
            camera.farClip = entityJson["CameraComponent"]["FarClip"];
            camera.isPrimary = entityJson["CameraComponent"]["IsPrimary"];
            registry.AddComponent(deserializedEntity, camera);
        }

        if (entityJson.contains("RenderComponent")) {
            RenderComponent renderable;
            // will eventually need an Asset Registry to map filepaths back to handles here
            renderable.modelHandle = entityJson["RenderComponent"]["ModelHandle"];
            renderable.shaderHandle = entityJson["RenderComponent"]["ShaderHandle"];
            registry.AddComponent(deserializedEntity, renderable);
        }
    }
    return true;
}