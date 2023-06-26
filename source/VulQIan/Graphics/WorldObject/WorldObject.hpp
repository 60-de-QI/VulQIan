// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "../Model/Model.hpp"
#include <memory>

namespace Vulqian::Engine::Graphics {

struct Transform2dComponent {
    glm::vec2 translation{}; // position offset
    glm::vec2 scale{1.f, 1.f};
    float rotation;

    glm::mat2 mat2() {
        const float s = glm::sin(rotation);
        const float c = glm::cos(rotation);

        glm::mat2 rotation_matrix{{c, s}, {-s, c}};
        glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};

        return rotation_matrix * scaleMat;
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
    glm::vec3 color{};
    Transform2dComponent transform_2d{};

  private:
    WorldObject(id_t _id) : id{_id} {}
    id_t id;
};

} // namespace Vulqian::Engine::Graphics
