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
Model::Model(Vulqian::Engine::Graphics::Device& device, const Data& data) : device(device), file_name(data.filepath) {
    this->create_vertex_buffers(data.vertices);
    this->create_index_buffers(data.indices);
}

void Model::create_vertex_buffers(const std::vector<Vertex>& vertices) {
    this->vertex_count = static_cast<uint32_t>(vertices.size());

    assert(this->vertex_count >= 3 && "Vertex count must be at least 3.");
    VkDeviceSize buffer_size = sizeof(vertices[0]) * this->vertex_count;
    uint32_t     vertex_size = sizeof(vertices[0]);

    Vulqian::Engine::Graphics::Buffer staging_buffer{
        this->device,
        vertex_size,
        this->vertex_count,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // host = cpu
    };
    staging_buffer.map();
    staging_buffer.writeToBuffer((void*)vertices.data()); // I hate void* but Vulkan requires them

    this->vertex_buffer = std::make_unique<Vulqian::Engine::Graphics::Buffer>(
        this->device,
        vertex_size,
        this->vertex_count,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT // host = cpu
    );

    this->device.copyBuffer(staging_buffer.getBuffer(), this->vertex_buffer->getBuffer(), buffer_size);
}

void Model::create_index_buffers(const std::vector<uint32_t>& indices) {
    this->index_count = static_cast<uint32_t>(indices.size());

    this->has_index_buffer = this->index_count > 0;

    if (!this->has_index_buffer) {
        return;
    }

    VkDeviceSize buffer_size = sizeof(indices[0]) * this->index_count;
    uint32_t     index_size = sizeof(indices[0]);

    Vulqian::Engine::Graphics::Buffer index_buffer{
        this->device,
        index_size,
        this->index_count,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // host = cpu
    };

    index_buffer.map();
    index_buffer.writeToBuffer((void*)indices.data()); // I hate void* but Vulkan requires them

    this->index_buffer = std::make_unique<Vulqian::Engine::Graphics::Buffer>(
        this->device,
        index_size,
        this->index_count,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT // host = cpu
    );

    this->device.copyBuffer(index_buffer.getBuffer(), this->index_buffer->getBuffer(), buffer_size);
}

std::unique_ptr<Model> Model::create_model_from_file(Vulqian::Engine::Graphics::Device& device, const std::string& filepath) {
    Data data{};
    data.load_model(filepath);

    return std::make_unique<Model>(device, data);
}

void Model::bind(VkCommandBuffer command_buffer) {
    std::array<VkBuffer, 1>     buffers = {this->vertex_buffer->getBuffer()};
    std::array<VkDeviceSize, 1> offsets = {0};
    vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers.data(), offsets.data());

    if (this->has_index_buffer) {
        vkCmdBindIndexBuffer(command_buffer, this->index_buffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }
}

void Model::draw(VkCommandBuffer command_buffer) const {
    if (this->has_index_buffer) {
        vkCmdDrawIndexed(command_buffer, this->index_count, 1, 0, 0, 0);
    } else {
        vkCmdDraw(command_buffer, this->vertex_count, 1, 0, 0);
    }
}

std::vector<VkVertexInputBindingDescription> Model::Vertex::get_binding_descriptions() {
    return {{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}};
    // binding, stride, inputrate
}

std::vector<VkVertexInputAttributeDescription> Model::Vertex::get_attribute_descriptions() {
    std::vector<VkVertexInputAttributeDescription> attribute_descriptions{};

    attribute_descriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
    attribute_descriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
    attribute_descriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
    attribute_descriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});

    return attribute_descriptions;
    // binding, location, format, offset
}

void Model::Data::load_model(const std::string& model_filepath) {
    tinyobj::attrib_t                attrib;
    std::vector<tinyobj::shape_t>    shapes;
    std::vector<tinyobj::material_t> materials;
    std::string                      warn;

    if (std::string err; !tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model_filepath.c_str())) {
        throw Vulqian::Exception::failed_to_load(warn + err);
    }

    vertices.clear();
    indices.clear();
    this->filepath = model_filepath;

    std::unordered_map<Vertex, uint32_t> unique_vertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            if (index.vertex_index >= 0) {
                vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]};

                vertex.color = {
                    attrib.colors[3 * index.vertex_index + 0],
                    attrib.colors[3 * index.vertex_index + 1],
                    attrib.colors[3 * index.vertex_index + 2]};
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

            if (!unique_vertices.contains(vertex)) {
                unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(unique_vertices[vertex]);
        }
    }
}

} // namespace Vulqian::Engine::Graphics
