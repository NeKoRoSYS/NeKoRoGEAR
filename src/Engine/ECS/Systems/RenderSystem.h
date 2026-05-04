#pragma once
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Components/Transform.h"
#include "Engine/ECS/Components/Renderable.h"
#include "Engine/ECS/Components/Camera.h"
#include <glm/glm.hpp>

namespace RenderSystem {
    inline void Update(Registry& registry) {
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        
        auto cameraArrayPtr = registry.GetComponentArray<CameraComponent>();
        auto& cameras = cameraArrayPtr->GetPackedArray();
        
        for (size_t i = 0; i < cameraArrayPtr->GetValidSize(); ++i) {
            if (cameras[i].isPrimary) {
                view = cameras[i].viewMatrix;
                projection = cameras[i].projectionMatrix;
                break;
            }
        }

        auto renderArrayPtr = registry.GetComponentArray<RenderComponent>();
        auto& renderables = renderArrayPtr->GetPackedArray();
        auto& indexToEntity = renderArrayPtr->GetIndexToEntityMap();

        for (size_t i = 0; i < renderArrayPtr->GetValidSize(); ++i) {
            auto& renderable = renderables[i];
            if (!renderable.mesh || !renderable.shader) continue;

            Entity entity = indexToEntity[i];
            auto& transform = registry.GetComponent<TransformComponent>(entity);

            renderable.shader->Use();
            renderable.shader->SetMat4("u_View", view);
            renderable.shader->SetMat4("u_Projection", projection);
            renderable.shader->SetMat4("u_Model", transform.modelMatrix);

            renderable.mesh->Draw();
        }
    }
}