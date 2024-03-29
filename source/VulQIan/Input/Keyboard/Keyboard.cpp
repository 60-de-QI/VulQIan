// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include <limits>

#include "Keyboard.hpp"

namespace Vulqian::Engine::Input {

void KeyboardMovementController::move_in_plane_xz(GLFWwindow* window, float dt,Vulqian::Engine::ECS::Components::Transform_TB_YXZ& transform) const {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    glm::vec3 rotate{0};

    if (glfwGetKey(window, this->keys.look_right) == GLFW_PRESS)
        rotate.y += 1.f;
    if (glfwGetKey(window, this->keys.look_left) == GLFW_PRESS)
        rotate.y -= 1.f;
    if (glfwGetKey(window, this->keys.look_up) == GLFW_PRESS)
        rotate.x += 1.f;
    if (glfwGetKey(window, this->keys.look_down) == GLFW_PRESS)
        rotate.x -= 1.f;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
        transform.rotation += this->look_speed * dt * glm::normalize(rotate);
    }

    transform.rotation.x = glm::clamp(transform.rotation.x, -1.5f, 1.5f);
    transform.rotation.y = glm::mod(transform.rotation.y, glm::two_pi<float>());

    // if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
    //     glm::vec3 rotationAxis = glm::normalize(rotate);
    //     float     rotationAngle = this->look_speed * dt * glm::length(rotate);
    //     glm::quat rotationDelta = glm::angleAxis(rotationAngle, rotationAxis);
    //     object.transform.rotation = rotationDelta * object.transform.rotation;
    // }

    transform.rotation.x = glm::clamp(transform.rotation.x, -1.5f, 1.5f);
    transform.rotation.y = glm::mod(transform.rotation.y, glm::two_pi<float>());

    float           yaw{transform.rotation.y};
    const glm::vec3 forward_dir{sin(yaw), 0.f, cos(yaw)};
    const glm::vec3 right_dir{forward_dir.z, 0.f, -forward_dir.x};
    const glm::vec3 up_dir{0.f, -1.f, 0.f};

    glm::vec3 move_dir{0.f};

    if (glfwGetKey(window, this->keys.move_forward) == GLFW_PRESS)
        move_dir += forward_dir;
    if (glfwGetKey(window, this->keys.move_backward) == GLFW_PRESS)
        move_dir -= forward_dir;
    if (glfwGetKey(window, this->keys.move_right) == GLFW_PRESS)
        move_dir += right_dir;
    if (glfwGetKey(window, this->keys.move_left) == GLFW_PRESS)
        move_dir -= right_dir;
    if (glfwGetKey(window, this->keys.move_up) == GLFW_PRESS)
        move_dir += up_dir;
    if (glfwGetKey(window, this->keys.move_down) == GLFW_PRESS)
        move_dir -= up_dir;

    if (glm::dot(move_dir, move_dir) > std::numeric_limits<float>::epsilon()) {
        transform.translation += this->move_speed * dt * glm::normalize(move_dir);
    }
}

} // namespace Vulqian::Engine::Input
