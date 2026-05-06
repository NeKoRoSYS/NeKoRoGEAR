#pragma once
#include "Scene.h"
#include <memory>
#include <string>

class SceneSerializer {
public:
    SceneSerializer(const std::shared_ptr<Scene>& scene);

    bool Serialize(const std::string& filepath);
    bool Deserialize(const std::string& filepath);

private:
    std::shared_ptr<Scene> m_Scene;
};