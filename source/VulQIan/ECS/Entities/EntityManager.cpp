// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include <cassert>

#include "EntityManager.hpp"

namespace Vulqian::Engine::ECS {
EntityManager::EntityManager() {
    // Initialize the queue with all possible entity IDs
    for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
        this->available_entities.push(entity);
    }
}

Entity EntityManager::create_entity() {
    assert(this->living_entity_count < MAX_ENTITIES && "too many entities in existence");

    // Take an ID from the front of the queue
    Entity id = this->available_entities.front();
    this->available_entities.pop();
    ++this->living_entity_count;

    return id;
}

void EntityManager::destroy_entity(Entity entity) {
    assert(entity < MAX_ENTITIES && "entity out of range");

    // Invalidate the destroyed entity's signature
    this->signatures[entity].reset();

    // Put the destroyed ID at the back of the queue
    this->available_entities.push(entity);
    --this->living_entity_count;
}

void EntityManager::set_signature(Entity entity, Signature signature) {
    assert(entity < MAX_ENTITIES && "entity out of range");

    // Put this entity's signature into the array
    this->signatures[entity] = signature;
}

Signature EntityManager::get_signature(Entity entity) {
    assert(entity < MAX_ENTITIES && "entity out of range");

    // Get this entity's signature from the array
    return this->signatures[entity];
}

} // namespace Vulqian::Engine::ECS
