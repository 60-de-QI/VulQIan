// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

namespace Vulqian::Engine::ECS::Components {

struct PointLight {
    float     lightIntensity{1.0f};
    glm::vec3 color{1.0f, 1.0f, 1.0f};
};

}  // namespace Vulqian::Engine::ECS::Components
