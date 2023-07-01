// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "../Types.hpp"

#include <set>

namespace Vulqian::Engine::ECS {

class System {
    public:
        std::set<Entity> entities;
};

} // namespace Vulqian::Engine::ECS
