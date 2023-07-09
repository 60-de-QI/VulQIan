// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "../Device/Device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

namespace Vulqian::Engine::Graphics {

class Model {
  public:
    struct Vertex {
        glm::vec3 position{};
        glm::vec3 color{};
        glm::vec3 normal{};
        glm::vec2 uv{};

        static std::vector<VkVertexInputBindingDescription>   get_binding_descriptions();
        static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();

        bool operator==(const Vertex& other) const {
            return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
        }
    };

    struct Data {
        std::vector<Vertex>   vertices{};
        std::vector<uint32_t> indices{};
        std::string           filepath;

        void load_model(const std::string& filepath);
    };

    Model(Vulqian::Engine::Graphics::Device& device, const Data& vertices);
    ~Model();

    // Since the class manages memory objects and vertex buffers it cannot be copied. We are in charge of memory management.
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    static std::unique_ptr<Model> create_model_from_file(Vulqian::Engine::Graphics::Device& device, const std::string& filepath);

    void bind(VkCommandBuffer command_buffer);
    void draw(VkCommandBuffer command_buffer) const;

    std::string get_file_name(void) const noexcept { return this->file_name; }

  private:
    void create_vertex_buffers(const std::vector<Vertex>& vertices);
    void create_index_buffers(const std::vector<uint32_t>& indices);

    Vulqian::Engine::Graphics::Device& device;

    VkBuffer       vertex_buffer;
    VkDeviceMemory vertex_buffer_memory;
    uint32_t       vertext_count;

    bool           has_index_buffer{false};
    VkBuffer       index_buffer;
    VkDeviceMemory index_buffer_memory;
    uint32_t       index_count;
    std::string    file_name;
};
} // namespace Vulqian::Engine::Graphics
