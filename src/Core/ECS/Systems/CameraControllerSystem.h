#pragma once
#include "Core/Components/InputManager.h"
#include "Core/ECS/ECS.h"
#include "Core/ECS/Components/Transform.h"
#include "Core/ECS/Components/Camera.h"

class CameraControllerSystem : public System {
private:
    bool isRelativeModeEnabled = false; 

public:
    void Update(Registry& registry, float deltaTime, bool isCameraActive) {
        if (!isCameraActive) return;
        float moveSpeed = 5.0f * deltaTime;
        float mouseSensitivity = 0.2f;
        InputManager& input = InputManager::Get();

        if (input.IsActionActive(InputAction::Sprint)) moveSpeed = 2.5f * deltaTime;

        for (Entity entity : entities) {
            auto& camera = registry.GetComponent<CameraComponent>(entity);
            if (!camera.isPrimary) continue;

            auto& transform = registry.GetComponent<TransformComponent>(entity);

            glm::vec3 front;
            front.x = cos(glm::radians(transform.rotation.y)) * cos(glm::radians(transform.rotation.x));
            front.y = sin(glm::radians(transform.rotation.x));
            front.z = sin(glm::radians(transform.rotation.y)) * cos(glm::radians(transform.rotation.x));
            front = glm::normalize(front);
            
            glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

            transform.rotation.y += input.GetAxisValue(InputAxis::LookRight) * mouseSensitivity;
            transform.rotation.x += input.GetAxisValue(InputAxis::LookUp) * mouseSensitivity;

            if (transform.rotation.x > 89.0f) transform.rotation.x = 89.0f;
            if (transform.rotation.x < -89.0f) transform.rotation.x = -89.0f;

            transform.position += front * (input.GetAxisValue(InputAxis::MoveForward) * moveSpeed);
            transform.position += right * (input.GetAxisValue(InputAxis::MoveRight) * moveSpeed);
            transform.position += up * (input.GetAxisValue(InputAxis::MoveUp) * moveSpeed);
        }
    }
};