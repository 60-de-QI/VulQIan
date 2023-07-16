// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "../../Graphics/WorldObject/WorldObject.hpp"
#include "../../Window/Window.hpp"

namespace Vulqian::Engine::Input {
class MouseCameraController {
  public:
    void update_camera_orientation(GLFWwindow* window, float dt, Vulqian::Engine::Graphics::WorldObject& object) const;

    // Tentative mouse implementation
    float mouse_sensitivity{.5f};
    bool  invert_mouse{false};
};
} // namespace Vulqian::Engine::Input
