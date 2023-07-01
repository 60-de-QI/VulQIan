// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "../../Graphics/Model/Model.hpp"

#include <memory>

namespace Vulqian::Engine::ECS::Components {

struct Mesh {
    std::shared_ptr<Vulqian::Engine::Graphics::Model> model{};
    glm::vec3                                         color{};
};

} // namespace Vulqian::Engine::ECS::Components
