#pragma once
#include "Engine/ECS/ECS.h"
#include "Engine/ECS/Components/Transform.h"
#include "Engine/ECS/Components/Renderable.h"
#include "Engine/ECS/Components/Camera.h"
#include "Engine/Assets/AssetManager.h"
#include "Engine/Assets/Model.h"
#include "Engine/Rendering/RenderCommand.h"
#include "Engine/Rendering/Shader.h"

class RenderSystem : public System {
public:
    void Update(Registry& registry) {
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

        for (Entity entity : entities) {
            auto& renderable = registry.GetComponent<RenderComponent>(entity);
            auto& transform = registry.GetComponent<TransformComponent>(entity);

            Model* model = AssetManager::Get().models.Get(renderable.modelHandle);
            Shader* shader = AssetManager::Get().shaders.Get(renderable.shaderHandle);
            
            if (!model || !shader) continue;

            shader->Use();
            shader->SetMat4("u_View", view);
            shader->SetMat4("u_Projection", projection);
            shader->SetMat4("u_Model", transform.modelMatrix);

            for (size_t j = 0; j < model->meshHandles.size(); ++j) {
                VertexArray* vao = AssetManager::Get().meshes.Get(model->meshHandles[j]);
                if (!vao) continue;

                if (j < renderable.textureHandles.size()) {
                    Texture* tex = AssetManager::Get().textures.Get(renderable.textureHandles[j]);
                    if (tex) tex->Bind(0);
                }

                vao->Bind();
                RenderCommand::DrawIndexed(vao);
            }
        }
    }
};