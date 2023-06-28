// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <array>
#include <unordered_map>

#include "Types.hpp"

namespace Vulqian::Engine::ECS {

class IComponentArray {
  public:
    virtual ~IComponentArray() = default;
    virtual void entity_destroyed(Entity entity) = 0;
};

template <typename T>
class ComponentArray : public IComponentArray {
  public:
    void insert_data(Entity entity, T component);
    void remove_data(Entity entity);
    void entity_destroyed(Entity entity) override;

    T& get_data(Entity entity);

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
