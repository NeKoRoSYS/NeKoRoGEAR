#pragma once
#include "Core/ECS/ECS.h"
#include "Core/ECS/Components/Tag.h"
#include "Core/ECS/Components/Transform.h"
#include <vector>
#include <algorithm>

class Scene {
public:
    Scene() {}
    ~Scene() {}

    Entity CreateEntity(const std::string& name = "New Entity") {
        Entity entity = registry.CreateEntity();
        registry.AddComponent(entity, TagComponent{ name });
        registry.AddComponent(entity, TransformComponent{});
        
        m_Entities.push_back(entity);
        return entity;
    }

    void DestroyEntity(Entity entity) {
        registry.DestroyEntity(entity);
        m_Entities.erase(std::remove(m_Entities.begin(), m_Entities.end(), entity), m_Entities.end());
    }

    Registry& GetRegistry() { return registry; }
    const std::vector<Entity>& GetEntities() const { return m_Entities; }

private:
    Registry registry;
    std::vector<Entity> m_Entities;
};