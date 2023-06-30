// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "../../Graphics/WorldObject/WorldObject.hpp"
#include "../../Window/Window.hpp"

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

    void move_in_plane_xz(GLFWwindow* window, float dt, Vulqian::Engine::Graphics::WorldObject& object);
    void update_camera_orientation(GLFWwindow* window, float dt, Vulqian::Engine::Graphics::WorldObject& object);

    KeyMappings keys{};
    float       move_speed{3.f};
    float       look_speed{1.5f};

    // Tentative mouse implementation
    float mouse_sensitivity{0.01f};
    bool  invert_mouse{false};
};

} // namespace Vulqian::Engine::Input
