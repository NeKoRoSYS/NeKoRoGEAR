#pragma once
#include "Core/ECS/ECS.h"
#include "Core/ECS/Components/Transform.h"
#include "Core/ECS/Components/Camera.h"
#include <glm/gtc/matrix_transform.hpp>

class CameraSystem : public System {
public:
    void Update(Registry& registry, float aspectRatio) {
        for (Entity entity : entities) {
            auto& camera = registry.GetComponent<CameraComponent>(entity);
            auto& transform = registry.GetComponent<TransformComponent>(entity);

            glm::vec3 front;
            front.x = cos(glm::radians(transform.rotation.y)) * cos(glm::radians(transform.rotation.x));
            front.y = sin(glm::radians(transform.rotation.x));
            front.z = sin(glm::radians(transform.rotation.y)) * cos(glm::radians(transform.rotation.x));
            front = glm::normalize(front);

            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            camera.viewMatrix = glm::lookAt(transform.position, transform.position + front, up);
            camera.projectionMatrix = glm::perspective(glm::radians(camera.fov), aspectRatio, camera.nearClip, camera.farClip);
        }
    }
};