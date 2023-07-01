// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include <cassert>

#include "ComponentArray.hpp"

namespace Vulqian::Engine::ECS {

template<typename T>
void ComponentArray<T>::insert_data(Entity entity, T component) {
    assert(this->entity_to_index_map.find(entity) == this->entity_to_index_map.end() && "component added to same entity more than once");

    // Put new entry at end and update the maps
    size_t newIndex = this->size;
    this->entity_to_index_map[entity] = newIndex;
    this->index_to_entity_map[newIndex] = entity;
    this->component_array[newIndex] = component;
    ++this->size;
}

template<typename T>
void ComponentArray<T>::remove_data(Entity entity) {
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

template <typename T>
T& ComponentArray<T>::get_data(Entity entity) {
    assert(this->entity_to_index_map.find(entity) != this->entity_to_index_map.end() && "Retrieving non-existent component.");

    // Return a reference to the entity's component
    return this->component_array[this->entity_to_index_map[entity]];
}

template <typename T>
void ComponentArray<T>::entity_destroyed(Entity entity) {
    if (this->entity_to_index_map.find(entity) != this->entity_to_index_map.end()) {
        // Remove the entity's component if it existed
        this->remove_data(entity);
    }
}

} // namespace Vulqian::Engine::ECS
