#pragma once
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Components/Transform.h"
#include "Engine/ECS/Components/Camera.h"
#include <SDL3/SDL.h>

namespace CameraControllerSystem {
    inline void Update(Registry& registry, SDL_Window* window, float deltaTime) {
        const bool* keys = SDL_GetKeyboardState(NULL);
        float moveSpeed = 5.0f * deltaTime;
        float mouseSensitivity = 0.2f;

        auto cameraArrayPtr = registry.GetComponentArray<CameraComponent>();
        auto& cameras = cameraArrayPtr->GetPackedArray();
        auto& indexToEntity = cameraArrayPtr->GetIndexToEntityMap();
        
        for (size_t i = 0; i < cameraArrayPtr->GetValidSize(); ++i) {
            if (!cameras[i].isPrimary) continue;

            Entity entity = indexToEntity[i];
            auto& transform = registry.GetComponent<TransformComponent>(entity);

            glm::vec3 front;
            front.x = cos(glm::radians(transform.rotation.y)) * cos(glm::radians(transform.rotation.x));
            front.y = sin(glm::radians(transform.rotation.x));
            front.z = sin(glm::radians(transform.rotation.y)) * cos(glm::radians(transform.rotation.x));
            front = glm::normalize(front);
            glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

            float mouseX, mouseY;
            Uint32 mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
            
            if (mouseState & SDL_BUTTON_RMASK) {
                SDL_SetWindowRelativeMouseMode(window, true);
                
                transform.rotation.y += mouseX * mouseSensitivity;
                transform.rotation.x -= mouseY * mouseSensitivity;

                if (transform.rotation.x > 89.0f) transform.rotation.x = 89.0f;
                if (transform.rotation.x < -89.0f) transform.rotation.x = -89.0f;
                
                if (keys[SDL_SCANCODE_W]) transform.position += front * moveSpeed;
                if (keys[SDL_SCANCODE_S]) transform.position -= front * moveSpeed;
                if (keys[SDL_SCANCODE_A]) transform.position -= right * moveSpeed;
                if (keys[SDL_SCANCODE_D]) transform.position += right * moveSpeed;
                if (keys[SDL_SCANCODE_E]) transform.position += glm::vec3(0.0f, 1.0f, 0.0f) * moveSpeed;
                if (keys[SDL_SCANCODE_Q]) transform.position -= glm::vec3(0.0f, 1.0f, 0.0f) * moveSpeed;
                
            } else {
                SDL_SetWindowRelativeMouseMode(window, false);
            }
        }
    }
}