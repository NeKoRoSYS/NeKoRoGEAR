#pragma once
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Components/Transform.h"
#include "Engine/ECS/Components/Camera.h"
#include <SDL3/SDL.h>

class CameraControllerSystem : public System {
private:
    bool isRelativeModeEnabled = false; 

public:
    void ToggleCameraMode(SDL_Window* window) {
        isRelativeModeEnabled = !isRelativeModeEnabled;
        SDL_SetWindowRelativeMouseMode(window, isRelativeModeEnabled);
    }

    void Update(Registry& registry, float deltaTime) {
        const bool* keys = SDL_GetKeyboardState(NULL);
        float moveSpeed = 5.0f * deltaTime;
        float mouseSensitivity = 0.2f;

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

            float mouseX, mouseY;
            SDL_GetRelativeMouseState(&mouseX, &mouseY); 

            if (isRelativeModeEnabled) {
                transform.rotation.y += mouseX * mouseSensitivity;
                transform.rotation.x -= mouseY * mouseSensitivity;

                if (transform.rotation.x > 89.0f) transform.rotation.x = 89.0f;
                if (transform.rotation.x < -89.0f) transform.rotation.x = -89.0f;

                if (keys[SDL_SCANCODE_LSHIFT]) moveSpeed = 2.5f * deltaTime;
                if (keys[SDL_SCANCODE_W]) transform.position += front * moveSpeed;
                if (keys[SDL_SCANCODE_S]) transform.position -= front * moveSpeed;
                if (keys[SDL_SCANCODE_A]) transform.position -= right * moveSpeed;
                if (keys[SDL_SCANCODE_D]) transform.position += right * moveSpeed;
                if (keys[SDL_SCANCODE_E]) transform.position += glm::vec3(0.0f, 1.0f, 0.0f) * moveSpeed;
                if (keys[SDL_SCANCODE_Q]) transform.position -= glm::vec3(0.0f, 1.0f, 0.0f) * moveSpeed;
            }
        }
    }
};