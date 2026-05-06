#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Core/Rendering/Buffer.h"
#include "Core/Rendering/Mesh.h"
#include "Core/Assets/Model.h"
#include "Core/Assets/AssetManager.h"
#include <memory>
#include <string>
#include <vector>
#include <iostream>

class ModelLoader {
public:
    static std::unique_ptr<Model> Load(const std::string& filepath) {
        Assimp::Importer importer;
        
        // aiProcess_Triangulate: Forces all polygons to be triangles (OpenGL needs this).
        // aiProcess_FlipUVs: Flips the Y-axis for textures, standard for OpenGL.
        // aiProcess_GenSmoothNormals: Calculates normals if the artist forgot to export them.
        const aiScene* scene = importer.ReadFile(filepath, 
            aiProcess_Triangulate | 
            aiProcess_FlipUVs | 
            aiProcess_GenSmoothNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ASSIMP ERROR: " << importer.GetErrorString() << "\n";
            return nullptr;
        }

        auto model = std::make_unique<Model>();

        for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
            aiMesh* mesh = scene->mMeshes[m];
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;

            for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
                Vertex vertex;
                
                vertex.position[0] = mesh->mVertices[i].x;
                vertex.position[1] = mesh->mVertices[i].y;
                vertex.position[2] = mesh->mVertices[i].z;

                if (mesh->HasNormals()) {
                    vertex.normal[0] = mesh->mNormals[i].x;
                    vertex.normal[1] = mesh->mNormals[i].y;
                    vertex.normal[2] = mesh->mNormals[i].z;
                }

                if (mesh->mTextureCoords[0]) {
                    vertex.texCoords[0] = mesh->mTextureCoords[0][i].x;
                    vertex.texCoords[1] = mesh->mTextureCoords[0][i].y;
                } else {
                    vertex.texCoords[0] = 0.0f;
                    vertex.texCoords[1] = 0.0f;
                }

                vertices.push_back(vertex);
            }

            for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++) {
                    indices.push_back(face.mIndices[j]); 
                }
            }

            auto vertexArray = VertexArray::Create();
            auto vertexBuffer = VertexBuffer::Create((float*)vertices.data(), vertices.size() * sizeof(Vertex));
            vertexArray->AddVertexBuffer(std::move(vertexBuffer));

            auto indexBuffer = IndexBuffer::Create(indices.data(), indices.size());
            vertexArray->SetIndexBuffer(std::move(indexBuffer));

            AssetHandle meshHandle = AssetManager::Get().meshes.Add(std::move(vertexArray));
            model->meshHandles.push_back(meshHandle);
        }

        return model;
    }
};