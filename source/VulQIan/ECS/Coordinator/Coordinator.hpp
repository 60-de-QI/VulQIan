// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <memory>

#include "../Components/ComponentManager.hpp"
#include "../Entities/EntityManager.hpp"
#include "../Systems/SystemManager.hpp"

namespace Vulqian::Engine::ECS {

class Coordinator {
  public:
    void init() {
        // Create pointers to each manager
        this->component_manager = std::make_unique<ComponentManager>();
        this->entity_manager = std::make_unique<EntityManager>();
        this->system_manager = std::make_unique<SystemManager>();
    }

    // Entity methods
    Entity create_entity() {
        return this->entity_manager->create_entity();
    }

    void destroy_entity(Entity entity) {
        this->entity_manager->destroy_entity(entity);
        this->component_manager->entity_destroyed(entity);
        this->system_manager->entity_destroyed(entity);
    }

    // Component methods
    template <typename T>
    void register_component() {
        this->component_manager->register_component<T>();
    }

    template <typename T>
    void add_component(Entity entity, T component) {
        this->component_manager->add_component<T>(entity, component);

        auto signature = entity_manager->get_signature(entity);
        signature.set(this->component_manager->get_component_type<T>(), true);
        this->entity_manager->set_signature(entity, signature);

        this->system_manager->entity_signature_changed(entity, signature);
    }

    template <typename T>
    void remove_component(Entity entity) {
        this->component_manager->remove_component<T>(entity);

        auto signature = entity_manager->get_signature(entity);
        signature.set(this->component_manager->get_component_type<T>(), false);
        this->entity_manager->set_signature(entity, signature);

        this->system_manager->entity_signature_changed(entity, signature);
    }

    template <typename T>
    T& get_component(Entity entity) {
        return this->component_manager->get_component<T>(entity);
    }

    template <typename T>
    ComponentType get_component_type() {
        return this->component_manager->get_component_type<T>();
    }

    // System methods
    template <typename T>
    std::shared_ptr<T> register_system() {
        return system_manager->register_system<T>();
    }

    template <typename T>
    void set_system_signature(Signature signature) {
        this->system_manager->set_signature<T>(signature);
    }

  private:
    std::unique_ptr<Vulqian::Engine::ECS::ComponentManager> component_manager;
    std::unique_ptr<Vulqian::Engine::ECS::EntityManager>    entity_manager;
    std::unique_ptr<Vulqian::Engine::ECS::SystemManager>    system_manager;
};

} // namespace Vulqian::Engine::ECS
