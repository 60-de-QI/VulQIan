// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "Mouse.hpp"

namespace Vulqian::Engine::Input {

void MouseCameraController::update_camera_orientation(GLFWwindow* window, float dt, Vulqian::Engine::Graphics::WorldObject& object) const {
    double xpos;
    double ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    static double last_xpos = xpos;
    static double last_ypos = ypos;

    double xoffset = xpos - last_xpos;
    double yoffset = last_ypos - ypos; // Reversed since y-coordinates go from bottom to top

    last_xpos = xpos;
    last_ypos = ypos;

    xoffset *= mouse_sensitivity * dt;
    yoffset *= mouse_sensitivity * dt;

    if (invert_mouse) {
        yoffset *= -1.0f;
    }

    object.transform.rotation.y += static_cast<float>(xoffset);
    object.transform.rotation.x += static_cast<float>(yoffset);

    object.transform.rotation.x = glm::clamp(object.transform.rotation.x, -1.5f, 1.5f);
    object.transform.rotation.y = glm::mod(object.transform.rotation.y, glm::two_pi<float>());
}

} // namespace Vulqian::Engine::Input
