#pragma once

#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <queue>
#include <cassert>

// 1. Core Type Definitions
using Entity = uint32_t;
const Entity MAX_ENTITIES = 5000;

// 2. The Component Array Interface
// We need a non-templated base class so the Registry can store a list of them
class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0;
};

// 3. The Packed Component Array (The secret to Data-Oriented Design)
template <typename T>
class ComponentArray : public IComponentArray {
private:
    // The tightly packed continuous memory containing actual component data
    std::vector<T> componentArray;

    // Maps an Entity ID to its position in the componentArray
    std::unordered_map<Entity, size_t> entityToIndexMap;

    // Maps an index in the componentArray back to its Entity ID
    std::unordered_map<size_t, Entity> indexToEntityMap;

    size_t validSize = 0; // How many valid components are in the array

public:
    void InsertData(Entity entity, T component) {
        assert(entityToIndexMap.find(entity) == entityToIndexMap.end() && "Component added to same entity more than once.");

        // Put new component at the end of the packed array
        size_t newIndex = validSize;
        entityToIndexMap[entity] = newIndex;
        indexToEntityMap[newIndex] = entity;

        if (newIndex >= componentArray.size()) {
            componentArray.push_back(component);
        } else {
            componentArray[newIndex] = component;
        }
        validSize++;
    }

    void RemoveData(Entity entity) {
        assert(entityToIndexMap.find(entity) != entityToIndexMap.end() && "Removing non-existent component.");

        // SWAP AND POP (The magic of DOD)
        // To keep the array packed, we move the LAST element into the deleted element's spot
        size_t indexOfRemovedEntity = entityToIndexMap[entity];
        size_t indexOfLastElement = validSize - 1;
        
        componentArray[indexOfRemovedEntity] = componentArray[indexOfLastElement];

        // Update the maps to point to the moved element
        Entity entityOfLastElement = indexToEntityMap[indexOfLastElement];
        entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
        indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

        entityToIndexMap.erase(entity);
        indexToEntityMap.erase(indexOfLastElement);
        validSize--;
    }

    T& GetData(Entity entity) {
        assert(entityToIndexMap.find(entity) != entityToIndexMap.end() && "Retrieving non-existent component.");
        return componentArray[entityToIndexMap[entity]];
    }

    std::vector<T>& GetPackedArray() { return componentArray; }
    std::unordered_map<size_t, Entity>& GetIndexToEntityMap() { return indexToEntityMap; }
    size_t GetValidSize() { return validSize; }

    void EntityDestroyed(Entity entity) override {
        if (entityToIndexMap.find(entity) != entityToIndexMap.end()) {
            RemoveData(entity);
        }
    }
};

// 4. The Registry (The Engine's Central Nervous System)
class Registry {
private:
    std::queue<Entity> availableEntities;
    uint32_t livingEntityCount = 0;

    // Stores all our different ComponentArrays
    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> componentArrays;

public:
    Registry() {
        for (Entity i = 0; i < MAX_ENTITIES; ++i) {
            availableEntities.push(i);
        }
    }
    
    template<typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray() {
        std::type_index typeName = std::type_index(typeid(T));
        assert(componentArrays.find(typeName) != componentArrays.end() && "Component not registered before use.");
        return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeName]);
    }

    Entity CreateEntity() {
        assert(livingEntityCount < MAX_ENTITIES && "Too many entities in existence.");
        Entity id = availableEntities.front();
        availableEntities.pop();
        livingEntityCount++;
        return id;
    }

    void DestroyEntity(Entity entity) {
        // Tell all component arrays that this entity died so they can clean up their data
        for (auto const& pair : componentArrays) {
            auto const& componentArray = pair.second;
            componentArray->EntityDestroyed(entity);
        }
        availableEntities.push(entity);
        livingEntityCount--;
    }

    template <typename T>
    void RegisterComponent() {
        std::type_index typeName = std::type_index(typeid(T));
        assert(componentArrays.find(typeName) == componentArrays.end() && "Registering component type more than once.");
        componentArrays[typeName] = std::make_shared<ComponentArray<T>>();
    }

    template <typename T>
    void AddComponent(Entity entity, T component) {
        GetComponentArray<T>()->InsertData(entity, component);
    }

    template <typename T>
    void RemoveComponent(Entity entity) {
        GetComponentArray<T>()->RemoveData(entity);
    }

    template <typename T>
    T& GetComponent(Entity entity) {
        return GetComponentArray<T>()->GetData(entity);
    }
};