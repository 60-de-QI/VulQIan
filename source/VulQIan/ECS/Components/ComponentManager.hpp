// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <cassert>
#include <memory>
#include <unordered_map>

#include "../Types.hpp"
#include "ComponentArray.hpp"
#include "Transform.hpp"
#include "Mesh.hpp"
#include "PointLight.hpp"

namespace Vulqian::Engine::ECS {

class ComponentManager {
  public:
    template <typename T>
    void register_component() {
        const char* type_name = typeid(T).name();

        assert(component_types.find(type_name) == component_types.end() && "Registering component type more than once.");

        // Add this component type to the component type map
        component_types.insert({type_name, this->next_component_type});

        // Create a ComponentArray pointer and add it to the component arrays map
        this->component_arrays.insert({type_name, std::make_shared<ComponentArray<T>>()});

        // Increment the value so that the next component registered will be different
        ++this->next_component_type;
    }

    template <typename T>
    ComponentType get_component_type() {
        const char* type_name = typeid(T).name();

        assert(component_types.find(type_name) != component_types.end() && "Component not registered before use.");

        // Return this component's type - used for creating signatures
        return component_types[type_name];
    }

    template <typename T>
    void add_component(Entity entity, T component) {
        // Add a component to the array for an entity
        this->get_component_array<T>()->insert_data(entity, component);
    }

    template <typename T>
    void remove_component(Entity entity) {
        // Remove a component from the array for an entity
        this->get_component_array<T>()->remove_data(entity);
    }

    template <typename T>
    T& get_component(Entity entity) {
        // Get a reference to a component from the array for an entity
        return this->get_component_array<T>()->get_data(entity);
    }

    void entity_destroyed(Entity entity) const {
        // Notify each component array that an entity has been destroyed
        // If it has a component for that entity, it will remove it
        for (auto const& [type, component] : this->component_arrays) {
            component->entity_destroyed(entity);
        }
    }

  private:
    // Map from type string pointer to a component type
    std::unordered_map<const char*, ComponentType> component_types{};

    // Map from type string pointer to a component array
    std::unordered_map<const char*, std::shared_ptr<IComponentArray>> component_arrays{};

    // The component type to be assigned to the next registered component - starting at 0
    ComponentType next_component_type{};

    // Convenience function to get the statically casted pointer to the ComponentArray of type T.
    template <typename T>
    std::shared_ptr<ComponentArray<T>> get_component_array() {
        const char* type_name = typeid(T).name();

        assert(component_types.find(type_name) != component_types.end() && "Component not registered before use.");

        return std::static_pointer_cast<ComponentArray<T>>(this->component_arrays[type_name]);
    }
};

} // namespace Vulqian::Engine::ECS
