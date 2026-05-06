#pragma once
#include "Core/ECS/ECS.h"
#include "Core/ECS/Components/Transform.h"
#include "Core/ECS/Components/Renderable.h"
#include "Core/ECS/Components/Camera.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Assets/Model.h"
#include "Core/Rendering/RenderCommand.h"
#include "Core/Rendering/Shader.h"

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
            shader->SetInt("u_Texture", 0);

            for (size_t j = 0; j < model->meshHandles.size(); ++j) {
                VertexArray* vao = AssetManager::Get().meshes.Get(model->meshHandles[j]);
                if (!vao) continue;
                
                AssetHandle texToBind = INVALID_ASSET_HANDLE;

                if (j < renderable.materialOverrides.size() && renderable.materialOverrides[j] != INVALID_ASSET_HANDLE) {
                    texToBind = renderable.materialOverrides[j];
                } else if (j < model->textureHandles.size() && model->textureHandles[j] != INVALID_ASSET_HANDLE) {
                    texToBind = model->textureHandles[j];
                }

                if (texToBind != INVALID_ASSET_HANDLE) {
                    Texture* tex = AssetManager::Get().textures.Get(texToBind);
                    if (tex) {
                        tex->Bind(0);
                    }
                }

                vao->Bind();
                RenderCommand::DrawIndexed(vao);
            }
        }
    }
};