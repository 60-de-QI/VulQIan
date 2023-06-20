// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "Model.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>

namespace Vulqian::Engine::Graphics {
Model::Model(Vulqian::Engine::Graphics::Device& device, const std::vector<Vertex>& vertices) : device(device) {
    this->create_vertex_buffers(vertices);
}

Model::~Model() {
    vkDestroyBuffer(this->device.get_device(), this->vertex_buffer, nullptr);
    vkFreeMemory(this->device.get_device(), this->vertex_buffer_memory, nullptr);
}

void Model::create_vertex_buffers(const std::vector<Vertex>& vertices) {
    this->vertext_count = static_cast<uint32_t>(vertices.size());

    assert(this->vertext_count >= 3 && "Vertex count must be at least 3.");
    VkDeviceSize buffer_size = sizeof(vertices[0]) * vertext_count;

    this->device.createBuffer(
        buffer_size,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // host = cpu
        this->vertex_buffer,
        this->vertex_buffer_memory);

    void* data;
    vkMapMemory(this->device.get_device(), this->vertex_buffer_memory, 0, buffer_size, 0, &data);
    std::copy(vertices.data(), vertices.data() + vertext_count, static_cast<Vertex*>(data));
    vkUnmapMemory(this->device.get_device(), this->vertex_buffer_memory);
}

void Model::bind(VkCommandBuffer command_buffer) {
    std::array<VkBuffer, 1> buffers = {this->vertex_buffer};
    std::array<VkDeviceSize, 1> offsets = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers.data(), offsets.data());
}

void Model::draw(VkCommandBuffer command_buffer) const {
    vkCmdDraw(command_buffer, this->vertext_count, 1, 0, 0);
}

std::vector<VkVertexInputBindingDescription> Model::Vertex::get_binding_descriptions() {
    return {{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}};
    // binding, stride, inputrate
}

std::vector<VkVertexInputAttributeDescription> Model::Vertex::get_attribute_descriptions() {
    return {
        {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, position)},
        {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)}
    };
    // binding, location, format, offset
}

} // namespace Vulqian::Engine::Graphics
