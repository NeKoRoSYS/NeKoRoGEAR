#pragma once
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Components/Transform.h"
#include "Engine/ECS/Components/Camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace CameraSystem {
    inline void Update(Registry& registry, float aspectRatio) {
        auto cameraArrayPtr = registry.GetComponentArray<CameraComponent>();
        auto& cameras = cameraArrayPtr->GetPackedArray();
        auto& indexToEntity = cameraArrayPtr->GetIndexToEntityMap();
        size_t size = cameraArrayPtr->GetValidSize();

        for (size_t i = 0; i < size; ++i) {
            auto& camera = cameras[i];
            Entity entity = indexToEntity[i];

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
}