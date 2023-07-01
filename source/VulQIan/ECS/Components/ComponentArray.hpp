// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <array>
#include <cassert>
#include <unordered_map>

#include "../Types.hpp"

namespace Vulqian::Engine::ECS {

class IComponentArray {
  public:
    virtual ~IComponentArray() = default;
    virtual void entity_destroyed(Entity entity) = 0;
};

template <typename T>
class ComponentArray : public IComponentArray {
  public:
    void insert_data(Entity entity, T component) {
        assert(this->entity_to_index_map.find(entity) == this->entity_to_index_map.end() && "component added to same entity more than once");

        // Put new entry at end and update the maps
        size_t newIndex = this->size;
        this->entity_to_index_map[entity] = newIndex;
        this->index_to_entity_map[newIndex] = entity;
        this->component_array[newIndex] = component;
        ++this->size;
    }

    void remove_data(Entity entity) {
        assert(this->entity_to_index_map.find(entity) != this->entity_to_index_map.end() && "removing non-existent component");

        // Copy element at end into deleted element's place to maintain density
        size_t indexOfRemovedEntity = this->entity_to_index_map[entity];
        size_t indexOfLastElement = this->size - 1;
        this->component_array[indexOfRemovedEntity] = this->component_array[indexOfLastElement];

        // Update map to point to moved spot
        Entity entityOfLastElement = this->index_to_entity_map[indexOfLastElement];
        this->entity_to_index_map[entityOfLastElement] = indexOfRemovedEntity;
        this->index_to_entity_map[indexOfRemovedEntity] = entityOfLastElement;

        this->entity_to_index_map.erase(entity);
        this->index_to_entity_map.erase(indexOfLastElement);

        --this->size;
    }

    void entity_destroyed(Entity entity) override {
        if (this->entity_to_index_map.find(entity) != this->entity_to_index_map.end()) {
            // Remove the entity's component if it existed
            this->remove_data(entity);
        }
    }

    T& get_data(Entity entity) {
        assert(this->entity_to_index_map.find(entity) != this->entity_to_index_map.end() && "Retrieving non-existent component.");

        // Return a reference to the entity's component
        return this->component_array[this->entity_to_index_map[entity]];
    }

  private:
    // The packed array of components (of generic type T),
    // set to a specified maximum amount, matching the maximum number
    // of entities allowed to exist simultaneously, so that each entity
    // has a unique spot.
    std::array<T, MAX_ENTITIES> component_array;

    // Map from an entity ID to an array index.
    std::unordered_map<Entity, size_t> entity_to_index_map;

    // Map from an array index to an entity ID.
    std::unordered_map<size_t, Entity> index_to_entity_map;

    // Total size of valid entries in the array.
    size_t size;
};

} // namespace Vulqian::Engine::ECS
