// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "../../Window/Window.hpp"
#include "../../ECS/ECS.hpp"

namespace Vulqian::Engine::Input {

class KeyboardMovementController {
  public:
    struct KeyMappings {
        int move_left = GLFW_KEY_A;
        int move_right = GLFW_KEY_D;
        int move_forward = GLFW_KEY_W;
        int move_backward = GLFW_KEY_S;
        int move_up = GLFW_KEY_E;
        int move_down = GLFW_KEY_Q;
        int look_left = GLFW_KEY_LEFT;
        int look_right = GLFW_KEY_RIGHT;
        int look_up = GLFW_KEY_UP;
        int look_down = GLFW_KEY_DOWN;
    };

    void move_in_plane_xz(GLFWwindow* window, float dt, Vulqian::Engine::ECS::Components::Transform_TB_YXZ& transform) const;

    KeyMappings keys{};
    float       move_speed{3.f};
    float       look_speed{1.5f};
};

} // namespace Vulqian::Engine::Input
