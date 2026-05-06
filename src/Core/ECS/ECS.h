// Core/ECS/ECS.h
#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <queue>
#include <cassert>
#include <algorithm>
#include <cstdint>
#include "Signature.h"

using Entity = uint32_t;
using ComponentTypeID = uint32_t;
using SystemTypeID = uint32_t;

inline ComponentTypeID GetUniqueComponentID() {
    static ComponentTypeID lastID = 0;
    return lastID++;
}

template <typename T>
inline ComponentTypeID GetComponentTypeID() {
    static ComponentTypeID typeID = GetUniqueComponentID();
    return typeID;
}

inline SystemTypeID GetUniqueSystemID() {
    static SystemTypeID lastID = 0;
    return lastID++;
}

template <typename T>
inline SystemTypeID GetSystemTypeID() {
    static SystemTypeID typeID = GetUniqueSystemID();
    return typeID;
}

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0;
};

template <typename T>
class ComponentArray : public IComponentArray {
private:
    std::vector<T> componentArray; 
    std::unordered_map<Entity, size_t> entityToIndex;
    std::vector<Entity> indexToEntity;

public:
    void InsertData(Entity entity, T component) {
        assert(entityToIndex.find(entity) == entityToIndex.end() && "Component added to same entity more than once.");

        size_t newIndex = componentArray.size();
        entityToIndex[entity] = newIndex;
        indexToEntity.push_back(entity);
        componentArray.push_back(component);
    }

    void RemoveData(Entity entity) {
        if (entityToIndex.find(entity) == entityToIndex.end()) return;

        size_t indexOfRemovedEntity = entityToIndex[entity];
        size_t indexOfLastElement = componentArray.size() - 1;

        componentArray[indexOfRemovedEntity] = std::move(componentArray[indexOfLastElement]);

        Entity entityOfLastElement = indexToEntity[indexOfLastElement];
        entityToIndex[entityOfLastElement] = indexOfRemovedEntity;
        indexToEntity[indexOfRemovedEntity] = entityOfLastElement;

        entityToIndex.erase(entity);
        indexToEntity.pop_back();
        componentArray.pop_back();
    }

    T& GetData(Entity entity) {
        assert(entityToIndex.find(entity) != entityToIndex.end() && "Retrieving non-existent component.");
        return componentArray[entityToIndex[entity]];
    }

    void EntityDestroyed(Entity entity) override {
        if (entityToIndex.find(entity) != entityToIndex.end()) {
            RemoveData(entity);
        }
    }
    
    std::vector<T>& GetPackedArray() { return componentArray; }
    size_t GetValidSize() { return componentArray.size(); }
};

class System {
public:
    std::vector<Entity> entities;
    Signature signature;
};

class Registry {
private:
    Entity nextEntityId = 0;
    std::queue<Entity> availableEntities;
    uint32_t livingEntityCount = 0;

    std::vector<std::shared_ptr<IComponentArray>> componentArrays;
    std::unordered_map<Entity, Signature> entitySignatures;
    std::vector<std::shared_ptr<System>> systems;

public:
    template<typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray() {
        ComponentTypeID typeId = GetComponentTypeID<T>();
        assert(typeId < componentArrays.size() && "Component not registered before use.");
        return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeId]);
    }

    Entity CreateEntity() {
        Entity id;
        if (!availableEntities.empty()) {
            id = availableEntities.front();
            availableEntities.pop();
        } else {
            id = nextEntityId++;
        }
        livingEntityCount++;
        return id;
    }

    void DestroyEntity(Entity entity) {
        for (auto const& pair : componentArrays) {
            if (pair) pair->EntityDestroyed(entity);
        }

        entitySignatures.erase(entity);
        Signature emptySignature;
        EntitySignatureChanged(entity, emptySignature);

        availableEntities.push(entity);
        livingEntityCount--;
    }

    template <typename T>
    void RegisterComponent() {
        ComponentTypeID typeId = GetComponentTypeID<T>();
        if (typeId >= componentArrays.size()) {
            componentArrays.resize(typeId + 1, nullptr);
        }
        componentArrays[typeId] = std::make_shared<ComponentArray<T>>();
    }

    template <typename T>
    bool HasComponent(Entity entity) {
        ComponentTypeID typeId = GetComponentTypeID<T>();
        auto it = entitySignatures.find(entity);
        if (it != entitySignatures.end()) {
            return it->second.test(typeId);
        }
        return false;
    }

    template <typename T>
    void AddComponent(Entity entity, T component) {
        GetComponentArray<T>()->InsertData(entity, component);

        ComponentTypeID typeId = GetComponentTypeID<T>();
        entitySignatures[entity].set(typeId);

        EntitySignatureChanged(entity, entitySignatures[entity]);
    }

    template <typename T>
    void RemoveComponent(Entity entity) {
        GetComponentArray<T>()->RemoveData(entity);

        ComponentTypeID typeId = GetComponentTypeID<T>();
        entitySignatures[entity].reset(typeId);

        EntitySignatureChanged(entity, entitySignatures[entity]);
    }

    template <typename T>
    T& GetComponent(Entity entity) {
        return GetComponentArray<T>()->GetData(entity);
    }

    template <typename T>
    std::shared_ptr<T> RegisterSystem() {
        SystemTypeID typeId = GetSystemTypeID<T>();
        if (typeId >= systems.size()) {
            systems.resize(typeId + 1, nullptr);
        }

        auto system = std::make_shared<T>();
        systems[typeId] = system;
        return system;
    }

    template <typename T>
    void SetSystemSignature(Signature signature) {
        SystemTypeID typeId = GetSystemTypeID<T>();
        assert(typeId < systems.size() && systems[typeId] != nullptr && "System used before registered.");
        systems[typeId]->signature = signature;
    }

private:
    void EntitySignatureChanged(Entity entity, const Signature& entitySignature) {
        for (auto const& system : systems) {
            if (!system) continue;

            bool matches = entitySignature.matches(system->signature);

            auto it = std::find(system->entities.begin(), system->entities.end(), entity);
            bool isAlreadyInSystem = it != system->entities.end();

            if (matches && !isAlreadyInSystem) {
                system->entities.push_back(entity);
            } else if (!matches && isAlreadyInSystem) {
                *it = system->entities.back();
                system->entities.pop_back();
            }
        }
    }
};