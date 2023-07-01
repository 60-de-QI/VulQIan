// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <bitset>
#include <cstdint>

namespace Vulqian::Engine::ECS {

using Entity = std::uint32_t;
const Entity MAX_ENTITIES = 500;

// Used to define the size of arrays later on
using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_ENTITIES>;

} // namespace Vulqian::Engine::ECS
