// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "../Components/Transform.hpp"
#include "../Coordinator/Coordinator.hpp"
#include "System.hpp"

#include <iostream>

namespace Vulqian::Engine::ECS::Systems {

class Physics : public System {
  public:
    void update() {
        for (auto const& entity : this->entities) {
            // auto& transform = coordinator.get_component<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>(entity);
        }
    }
};

} // namespace Vulqian::Engine::ECS::Systems
