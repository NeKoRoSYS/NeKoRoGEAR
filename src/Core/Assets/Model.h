#pragma once
#include <vector>
#include <cstdint>

using AssetHandle = uint32_t;

struct Model {
    std::vector<AssetHandle> meshHandles;
};