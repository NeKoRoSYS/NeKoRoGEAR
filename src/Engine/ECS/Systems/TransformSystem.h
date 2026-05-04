#pragma once
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Components/Transform.h"

namespace TransformSystem {

    inline void Update(Registry& registry) {
        auto transformArrayPtr = registry.GetComponentArray<TransformComponent>();
        auto& transforms = transformArrayPtr->GetPackedArray();
        size_t size = transformArrayPtr->GetValidSize();

        for (size_t i = 0; i < size; ++i) {
            auto& transform = transforms[i];

            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, transform.position);

            model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

            model = glm::scale(model, transform.scale);

            transform.modelMatrix = model;
        }
    }

}