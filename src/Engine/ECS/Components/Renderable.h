#pragma once
#include "Engine/Assets/AssetManager.h"

struct RenderComponent {
    AssetHandle modelHandle = INVALID_ASSET_HANDLE;
    AssetHandle shaderHandle = INVALID_ASSET_HANDLE;
    std::vector<AssetHandle> textureHandles;
};