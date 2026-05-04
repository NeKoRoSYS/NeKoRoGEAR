#pragma once

#include <glm/glm.hpp>

struct CameraComponent {
    float fov = 70.0f;
    float nearClip = 0.01f;
    float farClip = 100.0f;
    
    glm::mat4 viewMatrix{1.0f};
    glm::mat4 projectionMatrix{1.0f};
    bool isPrimary = true;
};