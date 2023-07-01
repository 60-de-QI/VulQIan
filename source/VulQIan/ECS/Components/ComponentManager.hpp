// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <cassert>
#include <memory>
#include <unordered_map>

#include "ComponentArray.hpp"
#include "../Types.hpp"

namespace Vulqian::Engine::ECS {

class ComponentManager {
  public:
    template <typename T>
    void register_component();

    template <typename T>
    ComponentType get_component_type();

    template <typename T>
    void add_component(Entity entity, T component);

    template <typename T>
    void remove_component(Entity entity);

    template <typename T>
    T& get_component(Entity entity);

    void entity_destroyed(Entity entity);

  private:
    // Map from type string pointer to a component type
    std::unordered_map<const char*, ComponentType> component_types{};

    // Map from type string pointer to a component array
    std::unordered_map<const char*, std::shared_ptr<IComponentArray>> component_arrays{};

    // The component type to be assigned to the next registered component - starting at 0
    ComponentType next_component_type{};

    // Convenience function to get the statically casted pointer to the ComponentArray of type T.
    template <typename T>
    std::shared_ptr<ComponentArray<T>> get_component_array();
};

} // namespace Vulqian::Engine::ECS
