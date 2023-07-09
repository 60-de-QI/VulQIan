// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <array>
#include <queue>

#include "../Types.hpp"

namespace Vulqian::Engine::ECS {
class EntityManager {
  public:
    EntityManager();

    Entity    create_entity();
    void      destroy_entity(Entity entity);
    void      set_signature(Entity entity, Signature const& signature);
    Signature get_signature(Entity entity);

  private:
    // Queue of unused entity IDs
    std::queue<Entity> available_entities{};

    // Array of signatures where the index corresponds to the entity ID
    std::array<Signature, MAX_ENTITIES> signatures{};

    // Total living entities - used to keep limits on how many exist
    uint32_t living_entity_count{};
};
} // namespace Vulqian::Engine::ECS
