// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "System.hpp"
#include "../Coordinator/Coordinator.hpp"
#include "../Components/Transform.hpp"

namespace Vulqian::Engine::ECS::Systems {
    
    class Physics : public System {
        void update() {
            for (auto const& entity: this->entities) {

            }
        }
    };

}
