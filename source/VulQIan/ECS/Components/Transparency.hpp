// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <glm/glm.hpp>

namespace Vulqian::Engine::ECS::Components {

struct Transparency {
    float     alpha{1.0f};                 // Alpha value (0.0 = fully transparent, 1.0 = fully opaque)
    glm::vec3 color{1.0f, 1.0f, 1.0f};     // Base color of the transparent object
    bool      requiresDepthSorting{true};  // Whether this object needs depth sorting for proper rendering
};

}  // namespace Vulqian::Engine::ECS::Components
