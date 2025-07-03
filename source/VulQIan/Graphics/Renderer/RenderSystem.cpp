// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "RenderSystem.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <map>

namespace Vulqian::Engine::Graphics {

struct SimplePushConstantData {
    glm::mat4 model_matrix{1.f};
    glm::mat4 normal_matrix{1.f};
    glm::vec4 color{1.f, 1.f, 1.f, 1.f};  // Add color/alpha for transparency
};

RenderSystem::RenderSystem(Vulqian::Engine::Graphics::Device& device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout) : device{device} {
    this->create_pipeline_layout(global_set_layout);
    this->create_pipeline(render_pass);
}

RenderSystem::~RenderSystem() {
    vkDestroyPipelineLayout(this->device.get_device(), this->pipeline_layout, nullptr);
}

void RenderSystem::create_pipeline_layout(VkDescriptorSetLayout global_set_layout) {
    VkPushConstantRange constant_range{};
    constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    constant_range.offset = 0;
    constant_range.size = sizeof(SimplePushConstantData);

    std::vector<VkDescriptorSetLayout> descriptor_set_layouts{global_set_layout};

    VkPipelineLayoutCreateInfo pipeline_create_info{};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_create_info.setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
    pipeline_create_info.pSetLayouts = descriptor_set_layouts.data();
    pipeline_create_info.pushConstantRangeCount = 1;
    pipeline_create_info.pPushConstantRanges = &constant_range;

    if (vkCreatePipelineLayout(this->device.get_device(), &pipeline_create_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_create("pipeline layout");
    }
}

void RenderSystem::create_pipeline(VkRenderPass render_pass) {
    assert(this->pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");

    Vulqian::Engine::Graphics::PipelineConstructInfo pipeline_info{};
    Vulqian::Engine::Graphics::Pipeline::get_default_config(pipeline_info);
    Vulqian::Engine::Graphics::Pipeline::enable_alpha_blending(pipeline_info);

    pipeline_info.render_pass = render_pass;
    pipeline_info.pipeline_layout = this->pipeline_layout;
    this->pipeline = std::make_unique<Vulqian::Engine::Graphics::Pipeline>(
        this->device,
        "./conan-build/Shaders/simple_shader.vert.spv",
        "./conan-build/Shaders/simple_shader.frag.spv",
        pipeline_info);
}

void RenderSystem::render_entities(Vulqian::Engine::Graphics::Frames::Info& frame_info, const std::vector<Vulqian::Engine::ECS::Entity>& entities, Vulqian::Engine::ECS::Coordinator& coordinator) {
    // Separate entities into opaque and transparent
    std::vector<Vulqian::Engine::ECS::Entity>     opaque_entities;
    std::map<float, Vulqian::Engine::ECS::Entity> transparent_entities;  // sorted by distance

    for (const auto& entity : entities) {
        // Only process entities that have BOTH Transform AND Mesh components
        if (coordinator.has_component<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>(entity) &&
            coordinator.has_component<Vulqian::Engine::ECS::Components::Mesh>(entity)) {
            if (coordinator.has_component<Vulqian::Engine::ECS::Components::Transparency>(entity)) {
                // Calculate distance from camera for depth sorting
                auto const& transform = coordinator.get_component<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>(entity);
                auto        offset = frame_info.camera.get_position() - transform.translation;
                float       distanceSquared = glm::dot(offset, offset);
                transparent_entities[distanceSquared] = entity;
            } else {
                opaque_entities.push_back(entity);
            }
        }
    }

    // Debug output only occasionally to reduce spam
    if (static int frame_counter = 0; ++frame_counter % 60 == 0) {  // Print every 60 frames (roughly once per second)
        std::cout << "Opaque entities: " << opaque_entities.size()
                  << ", Transparent entities: " << transparent_entities.size() << std::endl;

        if (!transparent_entities.empty()) {
            auto const& entity = transparent_entities.begin()->second;
            auto const& transform = coordinator.get_component<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>(entity);
            auto const& transparency = coordinator.get_component<Vulqian::Engine::ECS::Components::Transparency>(entity);
            std::cout << "Transparent quad position: ("
                      << transform.translation.x << ", "
                      << transform.translation.y << ", "
                      << transform.translation.z << "), Alpha: "
                      << transparency.alpha << std::endl;
        }
    }

    this->pipeline->bind(frame_info.command_buffer);

    vkCmdBindDescriptorSets(
        frame_info.command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        this->pipeline_layout,
        0,
        1,
        &frame_info.global_descriptor_set,
        0,
        nullptr);

    // Render opaque objects first
    for (const auto& entity : opaque_entities) {
        render_single_entity(frame_info, entity, coordinator);
    }

    // Render transparent objects back-to-front (farthest to nearest)
    for (auto it = transparent_entities.rbegin(); it != transparent_entities.rend(); ++it) {
        render_single_entity(frame_info, it->second, coordinator);
    }
}

void RenderSystem::render_single_entity(Vulqian::Engine::Graphics::Frames::Info& frame_info, Vulqian::Engine::ECS::Entity entity, Vulqian::Engine::ECS::Coordinator& coordinator) {
    auto const& mesh = coordinator.get_component<Vulqian::Engine::ECS::Components::Mesh>(entity);
    auto&       transform = coordinator.get_component<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>(entity);

    // Update rotation for colored cubes
    if (mesh.model->get_file_name() == Vulqian::Engine::Utils::colored_cube) {
        transform.rotation.y = glm::mod(transform.rotation.y + 0.001f, glm::two_pi<float>());
        transform.rotation.x = glm::mod(transform.rotation.x + 0.0005f, glm::two_pi<float>());
    }

    // Prepare push constants
    SimplePushConstantData push{};
    push.model_matrix = transform.mat4();
    push.normal_matrix = transform.normal_matrix();

    // Handle transparency if present
    if (coordinator.has_component<Vulqian::Engine::ECS::Components::Transparency>(entity)) {
        auto const& transparency = coordinator.get_component<Vulqian::Engine::ECS::Components::Transparency>(entity);
        push.color = glm::vec4(transparency.color, transparency.alpha);
    } else {
        push.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);  // Default opaque white
    }

    // Push constants to shader
    vkCmdPushConstants(
        frame_info.command_buffer,
        this->pipeline_layout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(SimplePushConstantData),
        &push);

    mesh.model->bind(frame_info.command_buffer);
    mesh.model->draw(frame_info.command_buffer);
}

void RenderSystem::render_opaque_entities_only(Vulqian::Engine::Graphics::Frames::Info&         frame_info,
                                               const std::vector<Vulqian::Engine::ECS::Entity>& entities,
                                               Vulqian::Engine::ECS::Coordinator&               coordinator) {
    this->pipeline->bind(frame_info.command_buffer);
    vkCmdBindDescriptorSets(
        frame_info.command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        this->pipeline_layout,
        0, 1,
        &frame_info.global_descriptor_set,
        0, nullptr);

    // Render only opaque entities
    for (const auto& entity : entities) {
        if (coordinator.has_component<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>(entity) &&
            coordinator.has_component<Vulqian::Engine::ECS::Components::Mesh>(entity) &&
            !coordinator.has_component<Vulqian::Engine::ECS::Components::Transparency>(entity)) {
            render_single_entity(frame_info, entity, coordinator);
        }
    }
}

void RenderSystem::render_transparent_entity(Vulqian::Engine::Graphics::Frames::Info& frame_info,
                                             Vulqian::Engine::ECS::Entity             entity,
                                             Vulqian::Engine::ECS::Coordinator&       coordinator) {
    this->pipeline->bind(frame_info.command_buffer);
    vkCmdBindDescriptorSets(
        frame_info.command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        this->pipeline_layout,
        0, 1,
        &frame_info.global_descriptor_set,
        0, nullptr);

    render_single_entity(frame_info, entity, coordinator);
}

}  // namespace Vulqian::Engine::Graphics
