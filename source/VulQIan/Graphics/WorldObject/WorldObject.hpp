// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <memory>

#include <glm/gtc/matrix_transform.hpp>

#include "../Model/Model.hpp"

namespace Vulqian::Engine::Graphics {

struct TransformComponent {
    glm::vec3 translation{};
    glm::vec3 scale{1.f, 1.f, 1.f};
    glm::vec3 rotation{};

    // Matrix = translation * Ry * Rx * Rz * scale (order is important as matrix multiplication on vectors is not commutative)
    // Rotation convention uses tait-bryan angles with axis order Y(1), X(2), Z(3)
    // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
    glm::mat4 mat4() const {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        return glm::mat4{
            {
                scale.x * (c1 * c3 + s1 * s2 * s3),
                scale.x * (c2 * s3),
                scale.x * (c1 * s2 * s3 - c3 * s1),
                0.0f,
            },
            {
                scale.y * (c3 * s1 * s2 - c1 * s3),
                scale.y * (c2 * c3),
                scale.y * (c1 * c3 * s2 + s1 * s3),
                0.0f,
            },
            {
                scale.z * (c2 * s1),
                scale.z * (-s2),
                scale.z * (c1 * c2),
                0.0f,
            },
            {translation.x, translation.y, translation.z, 1.0f}};
    }
};

class WorldObject {
  public:
    using id_t = unsigned int;

    static WorldObject create_game_object() {
        static id_t current_id{0};
        return WorldObject{current_id++};
    }

    // Deletion of copy constructor but keeping the move
    WorldObject(const WorldObject&) = delete;
    WorldObject& operator=(const WorldObject&) = delete;
    WorldObject(WorldObject&&) = default;
    WorldObject& operator=(WorldObject&&) = default;

    id_t get_id(void) const noexcept { return this->id; }

    std::shared_ptr<Vulqian::Engine::Graphics::Model> model{};
    glm::vec3                                         color{};
    TransformComponent                                transform{};

  private:
    explicit WorldObject(id_t _id) : id{_id} {}
    id_t id;
};

} // namespace Vulqian::Engine::Graphics
