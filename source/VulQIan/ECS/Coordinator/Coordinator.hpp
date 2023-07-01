// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "../Components/ComponentManager.hpp"
#include "../Entities/EntityManager.hpp"
#include "../Systems/SystemManager.hpp"

namespace Vulqian::Engine::ECS {

class Coordinator {
  public:
    void init();

    // Entity methods
    Entity create_entity();

    void destroy_entity(Entity entity);

    // Component methods
    template <typename T>
    void register_component();

    template <typename T>
    void add_component(Entity entity, T component);

    template <typename T>
    void remove_component(Entity entity);

    template <typename T>
    T& get_component(Entity entity);

    template <typename T>
    ComponentType get_component_type();

    // System methods
    template <typename T>
    std::shared_ptr<T> register_system();

    template <typename T>
    void set_system_signature(Signature signature);

  private:
    std::unique_ptr<ComponentManager> component_manager;
    std::unique_ptr<EntityManager>    entity_manager;
    std::unique_ptr<SystemManager>    system_manager;
};

} // namespace Vulqian::Engine::ECS
