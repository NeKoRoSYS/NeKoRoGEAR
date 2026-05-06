#pragma once

#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <queue>
#include <cassert>
#include <array> 
#include <bitset>
#include <algorithm>

using Entity = uint32_t;
const Entity MAX_ENTITIES = 5000;
const uint32_t MAX_COMPONENTS = 32;
using ComponentTypeID = uint32_t;
using Signature = std::bitset<MAX_COMPONENTS>;

inline ComponentTypeID GetUniqueComponentID() {
    static ComponentTypeID lastID = 0;
    return lastID++;
}

template <typename T>
inline ComponentTypeID GetComponentTypeID() {
    static ComponentTypeID typeID = GetUniqueComponentID();
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

    std::array<size_t, MAX_ENTITIES> entityToIndexArray; 
    std::array<Entity, MAX_ENTITIES> indexToEntityArray;

    size_t validSize = 0; // How many valid components are in the array

public:
    ComponentArray() {
        entityToIndexArray.fill(-1);
    }

    void InsertData(Entity entity, T component) {
        size_t newIndex = validSize;
        entityToIndexArray[entity] = newIndex;
        indexToEntityArray[newIndex] = entity;
        
        if (newIndex >= componentArray.size()) {
            componentArray.push_back(component);
        } else {
            componentArray[newIndex] = component;
        }
        validSize++;
    }

    void RemoveData(Entity entity) {
        size_t indexOfRemovedEntity = entityToIndexArray[entity];
        size_t indexOfLastElement = validSize - 1;
        
        componentArray[indexOfRemovedEntity] = componentArray[indexOfLastElement];
        
        Entity entityOfLastElement = indexToEntityArray[indexOfLastElement];
        entityToIndexArray[entityOfLastElement] = indexOfRemovedEntity;
        indexToEntityArray[indexOfRemovedEntity] = entityOfLastElement;
        
        entityToIndexArray[entity] = -1; 
        validSize--;
    }

    T& GetData(Entity entity) {
        return componentArray[entityToIndexArray[entity]];
    }

    std::vector<T>& GetPackedArray() { return componentArray; }

    std::array<Entity, MAX_ENTITIES>& GetIndexToEntityArray() { return indexToEntityArray; }
    
    size_t GetValidSize() { return validSize; }
    
    void EntityDestroyed(Entity entity) override {
        if (entityToIndexArray[entity] != (size_t)-1) {
            RemoveData(entity);
        }
    }
};

class System {
public:
    std::vector<Entity> entities;
    Signature signature;
};

class Registry {
private:
    std::queue<Entity> availableEntities;
    uint32_t livingEntityCount = 0;

    std::array<std::shared_ptr<IComponentArray>, MAX_COMPONENTS> componentArrays;

    std::array<Signature, MAX_ENTITIES> entitySignatures;
    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

public:
    Registry() {
        for (Entity i = 0; i < MAX_ENTITIES; ++i) {
            availableEntities.push(i);
            entitySignatures[i].reset();
        }
    }
    
    template<typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray() {
        ComponentTypeID typeId = GetComponentTypeID<T>();
        return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeId]);
    }

    Entity CreateEntity() {
        assert(livingEntityCount < MAX_ENTITIES && "Too many entities in existence.");
        Entity id = availableEntities.front();
        availableEntities.pop();
        livingEntityCount++;
        return id;
    }

    void DestroyEntity(Entity entity) {
        for (auto const& pair : componentArrays) {
            if (pair) pair->EntityDestroyed(entity);
        }
        
        entitySignatures[entity].reset();
        EntitySignatureChanged(entity, entitySignatures[entity]);
        
        availableEntities.push(entity);
        livingEntityCount--;
    }

    template <typename T>
    void RegisterComponent() {
        ComponentTypeID typeId = GetComponentTypeID<T>();
        componentArrays[typeId] = std::make_shared<ComponentArray<T>>();
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
        std::type_index typeName = std::type_index(typeid(T));
        assert(systems.find(typeName) == systems.end() && "Registering system more than once.");
        
        auto system = std::make_shared<T>();
        systems[typeName] = system;
        return system;
    }

    template <typename T>
    void SetSystemSignature(Signature signature) {
        std::type_index typeName = std::type_index(typeid(T));
        assert(systems.find(typeName) != systems.end() && "System used before registered.");
        systems[typeName]->signature = signature;
    }

private:
    void EntitySignatureChanged(Entity entity, Signature entitySignature) {
        for (auto const& pair : systems) {
            auto const& type = pair.first;
            auto const& system = pair.second;
            auto const& systemSignature = system->signature;

            bool matches = (entitySignature & systemSignature) == systemSignature;
            
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