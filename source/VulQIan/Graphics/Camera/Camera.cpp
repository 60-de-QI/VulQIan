// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include <cassert>
#include <limits>

#include "Camera.hpp"

namespace Vulqian::Engine::Graphics {

void Camera::set_orthographic_projection(float left, float right, float top, float bottom, float near, float far) {
    this->projection_matrix = glm::mat4{1.0f};
    this->projection_matrix[0][0] = 2.f / (right - left);
    this->projection_matrix[1][1] = 2.f / (bottom - top);
    this->projection_matrix[2][2] = 1.f / (far - near);
    this->projection_matrix[3][0] = -(right + left) / (right - left);
    this->projection_matrix[3][1] = -(bottom + top) / (bottom - top);
    this->projection_matrix[3][2] = -near / (far - near);
}

void Camera::set_perspective_projection(float fov_y, float aspect, float near, float far) {
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);

    const float tanHalfFovy = tan(fov_y / 2.f);
    this->projection_matrix = glm::mat4{0.0f};
    this->projection_matrix[0][0] = 1.f / (aspect * tanHalfFovy);
    this->projection_matrix[1][1] = 1.f / (tanHalfFovy);
    this->projection_matrix[2][2] = far / (far - near);
    this->projection_matrix[2][3] = 1.f;
    this->projection_matrix[3][2] = -(far * near) / (far - near);
}

void Camera::set_view_direction(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
    // Orthonormal basis construction
    const glm::vec3 w{glm::normalize(direction)};
    const glm::vec3 u{glm::normalize(glm::cross(w, up))};
    const glm::vec3 v{glm::cross(w, u)};

    this->view_matrix = glm::mat4{1.f};
    this->view_matrix[0][0] = u.x;
    this->view_matrix[1][0] = u.y;
    this->view_matrix[2][0] = u.z;
    this->view_matrix[0][1] = v.x;
    this->view_matrix[1][1] = v.y;
    this->view_matrix[2][1] = v.z;
    this->view_matrix[0][2] = w.x;
    this->view_matrix[1][2] = w.y;
    this->view_matrix[2][2] = w.z;
    this->view_matrix[3][0] = -glm::dot(u, position);
    this->view_matrix[3][1] = -glm::dot(v, position);
    this->view_matrix[3][2] = -glm::dot(w, position);
}

void Camera::set_view_target(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
    assert(glm::length(target) != 0.0f && "set View Target is equal to 0"); // Assertion to check if target is not zero

    this->set_view_direction(position, target - position, up);
}

void Camera::set_view_YXZ(glm::vec3 position, glm::vec3 rotation) {
    const float     c3 = glm::cos(rotation.z);
    const float     s3 = glm::sin(rotation.z);
    const float     c2 = glm::cos(rotation.x);
    const float     s2 = glm::sin(rotation.x);
    const float     c1 = glm::cos(rotation.y);
    const float     s1 = glm::sin(rotation.y);

    const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
    const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
    const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};

    this->view_matrix = glm::mat4{1.f};
    this->view_matrix[0][0] = u.x;
    this->view_matrix[1][0] = u.y;
    this->view_matrix[2][0] = u.z;
    this->view_matrix[0][1] = v.x;
    this->view_matrix[1][1] = v.y;
    this->view_matrix[2][1] = v.z;
    this->view_matrix[0][2] = w.x;
    this->view_matrix[1][2] = w.y;
    this->view_matrix[2][2] = w.z;
    this->view_matrix[3][0] = -glm::dot(u, position);
    this->view_matrix[3][1] = -glm::dot(v, position);
    this->view_matrix[3][2] = -glm::dot(w, position);
}

} // namespace Vulqian::Engine::Graphics
