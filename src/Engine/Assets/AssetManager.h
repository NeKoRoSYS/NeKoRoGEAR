#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <filesystem>
#include <iostream>
#include <functional>

using AssetHandle = uint32_t;
constexpr AssetHandle INVALID_ASSET_HANDLE = 0;

struct AssetMetadata {
    std::string filepath;
    std::filesystem::file_time_type lastWriteTime;
};

template<typename T>
class AssetPool {
public:
    AssetHandle Add(std::unique_ptr<T> asset, const std::string& filepath = "") {
        AssetHandle handle = ++nextHandle;
        assets[handle] = std::move(asset);
        
        if (!filepath.empty()) {
            metadata[handle] = { filepath, std::filesystem::last_write_time(filepath) };
        }
        return handle;
    }

    T* Get(AssetHandle handle) {
        auto it = assets.find(handle);
        if (it != assets.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    void Remove(AssetHandle handle) {
        assets.erase(handle);
        metadata.erase(handle);
    }

    void CheckForReloads(const std::function<std::unique_ptr<T>(const std::string&)>& reloadFunc) {
        for (auto& [handle, meta] : metadata) {
            try {
                auto currentWriteTime = std::filesystem::last_write_time(meta.filepath);
                if (currentWriteTime > meta.lastWriteTime) {
                    std::cout << "Hot-reloading asset: " << meta.filepath << std::endl;
                    assets[handle] = reloadFunc(meta.filepath);
                    meta.lastWriteTime = currentWriteTime;
                }
            } catch (const std::filesystem::filesystem_error& e) {
            }
        }
    }

private:
    std::unordered_map<AssetHandle, std::unique_ptr<T>> assets;
    std::unordered_map<AssetHandle, AssetMetadata> metadata;
    AssetHandle nextHandle = 0;
};

#include "Engine/Rendering/Buffer.h"
#include "Engine/Rendering/Shader.h"
#include "Engine/Rendering/Texture.h"
#include "Model.h"

class AssetManager {
public:
    static AssetManager& Get() {
        static AssetManager instance;
        return instance;
    }

    AssetPool<VertexArray> meshes;
    AssetPool<Shader> shaders;
    AssetPool<Texture> textures; 
    AssetPool<Model> models; 

    void Update() {
        shaders.CheckForReloads([](const std::string& filepath) {
            return std::make_unique<Shader>(filepath + ".vert", filepath + ".frag"); 
        });
    }

private:
    AssetManager() = default;
};