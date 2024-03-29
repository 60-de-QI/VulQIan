// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "../Types.hpp"
#include "Physics.hpp"
#include "System.hpp"

#include <cassert>
#include <memory>
#include <unordered_map>

namespace Vulqian::Engine::ECS {

class SystemManager {
  public:
    template <typename T>
    std::shared_ptr<T> register_system() {
        const char* typeName = typeid(T).name();

        assert(systems.find(typeName) == systems.end() && "Registering system more than once.");

        // Create a pointer to the system and return it so it can be used externally
        auto system = std::make_shared<T>();
        systems.insert({typeName, system});
        return system;
    }

    template <typename T>
    void set_signature(Signature signature) {
        const char* typeName = typeid(T).name();

        assert(systems.find(typeName) != systems.end() && "System used before registered.");

        // Set the signature for this system
        signatures.try_emplace(typeName, signature);
    }

    void entity_destroyed(Entity entity) const {
        // Erase a destroyed entity from all system lists
        // entities is a set so no check needed
        for (auto const& [type, system_ptr] : this->systems) {
            auto const& system = system_ptr;

            system->entities.erase(entity);
        }
    }

    void entity_signature_changed(Entity entity, Signature const& entitySignature) {
        // Notify each system that an entity's signature changed
        for (auto const& [type, system] : systems) {
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

  private:
    // Map from system type string pointer to a signature
    std::unordered_map<const char*, Signature> signatures{};

    // Map from system type string pointer to a system pointer
    std::unordered_map<const char*, std::shared_ptr<System>> systems{};
};

} // namespace Vulqian::Engine::ECS
