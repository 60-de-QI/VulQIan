// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "Model.hpp"
#include "../../Exception/Exception.hpp"
#include "../../Utils/Utils.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <unordered_map>

namespace std {
template <>
struct hash<Vulqian::Engine::Graphics::Model::Vertex> {
    size_t operator()(Vulqian::Engine::Graphics::Model::Vertex const& vertex) const {
        size_t seed = 0;

        Vulqian::Engine::Utils::hash_combine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);

        return seed;
    }
};

} // namespace std

namespace Vulqian::Engine::Graphics {
Model::Model(Vulqian::Engine::Graphics::Device& device, const Data& data) : device(device) {
    this->create_vertex_buffers(data.vertices);
    this->create_index_buffers(data.indices);

    this->file_name = data.filepath;
}

Model::~Model() {
    vkDestroyBuffer(this->device.get_device(), this->vertex_buffer, nullptr);
    vkFreeMemory(this->device.get_device(), this->vertex_buffer_memory, nullptr);

    if (this->has_index_buffer) {
        vkDestroyBuffer(this->device.get_device(), this->index_buffer, nullptr);
        vkFreeMemory(this->device.get_device(), this->index_buffer_memory, nullptr);
    }
}

void Model::create_vertex_buffers(const std::vector<Vertex>& vertices) {
    this->vertext_count = static_cast<uint32_t>(vertices.size());

    assert(this->vertext_count >= 3 && "Vertex count must be at least 3.");
    VkDeviceSize   buffer_size = sizeof(vertices[0]) * this->vertext_count;
    VkBuffer       staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    this->device.createBuffer(
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // host = cpu
        staging_buffer,
        staging_buffer_memory);

    void* data;
    vkMapMemory(this->device.get_device(), staging_buffer_memory, 0, buffer_size, 0, &data);
    std::copy(vertices.data(), vertices.data() + vertext_count, static_cast<Vertex*>(data));
    vkUnmapMemory(this->device.get_device(), staging_buffer_memory);

    this->device.createBuffer(
        buffer_size,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, // host = cpu
        this->vertex_buffer,
        this->vertex_buffer_memory);

    this->device.copyBuffer(staging_buffer, this->vertex_buffer, buffer_size);

    vkDestroyBuffer(this->device.get_device(), staging_buffer, nullptr);
    vkFreeMemory(this->device.get_device(), staging_buffer_memory, nullptr);
}

void Model::create_index_buffers(const std::vector<uint32_t>& indices) {
    this->index_count = static_cast<uint32_t>(indices.size());

    this->has_index_buffer = this->index_count > 0;

    if (!this->has_index_buffer) {
        return;
    }

    VkDeviceSize buffer_size = sizeof(indices[0]) * this->index_count;

    VkBuffer       staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    this->device.createBuffer(
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // host = cpu
        staging_buffer,
        staging_buffer_memory);

    void* data;
    vkMapMemory(this->device.get_device(), staging_buffer_memory, 0, buffer_size, 0, &data);
    // memcpy(data, indices.data(), static_cast<size_t>(buffer_size));
    std::copy(indices.begin(), indices.end(), static_cast<uint32_t*>(data));
    vkUnmapMemory(this->device.get_device(), staging_buffer_memory);

    this->device.createBuffer(
        buffer_size,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, // host = cpu
        this->index_buffer,
        this->index_buffer_memory);

    this->device.copyBuffer(staging_buffer, this->index_buffer, buffer_size);

    vkDestroyBuffer(this->device.get_device(), staging_buffer, nullptr);
    vkFreeMemory(this->device.get_device(), staging_buffer_memory, nullptr);
}

std::unique_ptr<Model> Model::create_model_from_file(Vulqian::Engine::Graphics::Device& device, const std::string& filepath) {
    Data data{};
    data.load_model(filepath);

    return std::make_unique<Model>(device, data);
}

void Model::bind(VkCommandBuffer command_buffer) {
    std::array<VkBuffer, 1>     buffers = {this->vertex_buffer};
    std::array<VkDeviceSize, 1> offsets = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers.data(), offsets.data());

    if (this->has_index_buffer) {
        vkCmdBindIndexBuffer(command_buffer, this->index_buffer, 0, VK_INDEX_TYPE_UINT32);
    }
}

void Model::draw(VkCommandBuffer command_buffer) const {
    if (this->has_index_buffer) {
        vkCmdDrawIndexed(command_buffer, this->index_count, 1, 0, 0, 0);
    } else {
        vkCmdDraw(command_buffer, this->vertext_count, 1, 0, 0);
    }
}

std::vector<VkVertexInputBindingDescription> Model::Vertex::get_binding_descriptions() {
    return {{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}};
    // binding, stride, inputrate
}

std::vector<VkVertexInputAttributeDescription> Model::Vertex::get_attribute_descriptions() {
    return {
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
        {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)}};
    // binding, location, format, offset
}

void Model::Data::load_model(const std::string& filepath) {
    tinyobj::attrib_t                attrib;
    std::vector<tinyobj::shape_t>    shapes;
    std::vector<tinyobj::material_t> materials;
    std::string                      warn;
    std::string                      err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
        throw Vulqian::Exception::failed_to_load(warn + err);
    }

    vertices.clear();
    indices.clear();
    this->filepath = filepath;

    std::unordered_map<Vertex, uint32_t> unique_vertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            if (index.vertex_index >= 0) {
                vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]};

                auto color_index = 3 * index.vertex_index + 2;
                if (color_index < attrib.colors.size()) {
                    vertex.color = {
                        attrib.vertices[color_index - 0],
                        attrib.vertices[color_index - 1],
                        attrib.vertices[color_index - 2]};
                } else {
                    vertex.color = {1.f, 1.f, 1.f}; // set default color
                }
            }

            if (index.normal_index >= 0) {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]};
            }

            if (index.texcoord_index >= 0) {
                vertex.uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]};
            }

            if (unique_vertices.count(vertex) == 0) {
                unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(unique_vertices[vertex]);
        }
    }
}

} // namespace Vulqian::Engine::Graphics
