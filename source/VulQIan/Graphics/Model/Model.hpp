// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "../Device/Device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>

namespace Vulqian::Engine::Graphics {

class Model {
   public:
    struct Vertex {
        glm::vec2 position;
        glm::vec3 color;

        static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
        static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();
    };

    Model(Vulqian::Engine::Graphics::Device &device, const std::vector<Vertex> &vertices);
    ~Model();

    // Since the class manages memory objects and vertex buffers it cannot be copied. We are in charge of memory management.
    Model(const Model &) = delete;
    Model &operator=(const Model &) = delete;

    void bind(VkCommandBuffer command_buffer);
    void draw(VkCommandBuffer command_buffer) const;

   private:
    void create_vertex_buffers(const std::vector<Vertex>& vertices);

    Vulqian::Engine::Graphics::Device &device;
    VkBuffer vertex_buffer;
    VkDeviceMemory vertex_buffer_memory;
    uint32_t vertext_count;
};
}  // namespace Vulqian::Graphics
