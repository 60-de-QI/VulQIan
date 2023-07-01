// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "SystemManager.hpp"

namespace Vulqian::Engine::ECS {

template <typename T>
std::shared_ptr<T> SystemManager::register_system() {
    const char* typeName = typeid(T).name();

    assert(systems.find(typeName) == systems.end() && "Registering system more than once.");

    // Create a pointer to the system and return it so it can be used externally
    auto system = std::make_shared<T>();
    systems.insert({typeName, system});
    return system;
}

template <typename T>
void SystemManager::set_signature(Signature signature) {
    const char* typeName = typeid(T).name();

    assert(systems.find(typeName) != systems.end() && "System used before registered.");

    // Set the signature for this system
    signatures.insert({typeName, signature});
}

void SystemManager::entity_destroyed(Entity entity) {
    // Erase a destroyed entity from all system lists
    // entities is a set so no check needed
    for (auto const& pair : this->systems) {
        auto const& system = pair.second;

        system->entities.erase(entity);
    }
}

void SystemManager::entity_signature_changed(Entity entity, Signature entitySignature) {
    // Notify each system that an entity's signature changed
    for (auto const& pair : systems) {
        auto const& type = pair.first;
        auto const& system = pair.second;
        auto const& systemSignature = signatures[type];

        // Entity signature matches system signature - insert into set
        if ((entitySignature & systemSignature) == systemSignature) {
            system->entities.insert(entity);
        }
        // Entity signature does not match system signature - erase from set
        else {
            system->entities.erase(entity);
        }
    }
}

} // namespace Vulqian::Engine::ECS
