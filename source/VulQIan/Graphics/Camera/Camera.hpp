// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Vulqian::Engine::Graphics {
class Camera {
  public:
    Camera() = default;
    ~Camera() = default;

    void set_orthographic_projection(float left, float right, float top, float bottom, float near, float far);
    void set_perspective_projection(float fov_y, float aspect, float near, float far);

    void set_view_direction(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
    void set_view_target(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
    void set_view_YXZ(glm::vec3 position, glm::vec3 rotation);

    const glm::mat4& get_projection() const noexcept { return this->projection_matrix; }
    const glm::mat4& get_view() const noexcept { return this->view_matrix; }

  private:
    glm::mat4 projection_matrix{1.f};
    glm::mat4 view_matrix{1.f};
};
} // namespace Vulqian::Engine::Graphics
