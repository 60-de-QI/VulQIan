// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "RenderSystem.hpp"

namespace Vulqian::Engine::Graphics {

struct SimplePushConstantData {
    glm::mat4 transform{1.f};
    glm::mat4 normal_matrix{1.f};
};

RenderSystem::RenderSystem(Vulqian::Engine::Graphics::Device& device, VkRenderPass render_pass) : device{device} {
    this->create_pipeline_layout();
    this->create_pipeline(render_pass);
}

RenderSystem::~RenderSystem() {
    vkDestroyPipelineLayout(this->device.get_device(), this->pipeline_layout, nullptr);
}

void RenderSystem::create_pipeline_layout() {
    VkPushConstantRange constant_range{};
    constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    constant_range.offset = 0;
    constant_range.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipeline_create_info{};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_create_info.setLayoutCount = 0;
    pipeline_create_info.pSetLayouts = nullptr;
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

    pipeline_info.render_pass = render_pass;
    pipeline_info.pipeline_layout = this->pipeline_layout;
    this->pipeline = std::make_unique<Vulqian::Engine::Graphics::Pipeline>(
        this->device,
        "./conan-build/Debug/Shaders/simple_shader.vert.spv",
        "./conan-build/Debug/Shaders/simple_shader.frag.spv",
        pipeline_info);
}

void RenderSystem::render_entities(Vulqian::Engine::Graphics::Frames::Info& frame_info, const std::vector<Vulqian::Engine::ECS::Entity>& entities, Vulqian::Engine::ECS::Coordinator& coordinator) {
    this->pipeline->bind(frame_info.command_buffer);
    auto                   projection_view = frame_info.camera.get_projection() * frame_info.camera.get_view();
    SimplePushConstantData push{};

    for (auto const& entity : entities) {
        push.transform = projection_view;

        if (coordinator.has_component<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>(entity)) {
            auto const& mesh = coordinator.get_component<Vulqian::Engine::ECS::Components::Mesh>(entity);
            auto&       transform = coordinator.get_component<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>(entity);

            if (mesh.model->get_file_name() == Vulqian::Engine::Utils::colored_cube) {
                transform.rotation.y = glm::mod(transform.rotation.y + 0.001f, glm::two_pi<float>());
                transform.rotation.x = glm::mod(transform.rotation.x + 0.0005f, glm::two_pi<float>());
            }

            push.transform = projection_view * transform.mat4();
            push.normal_matrix = transform.normal_matrix();
        }

        vkCmdPushConstants(
            frame_info.command_buffer,
            this->pipeline_layout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push);

        if (coordinator.has_component<Vulqian::Engine::ECS::Components::Mesh>(entity)) {
            auto const& mesh = coordinator.get_component<Vulqian::Engine::ECS::Components::Mesh>(entity);
            mesh.model->bind(frame_info.command_buffer);
            mesh.model->draw(frame_info.command_buffer);
        }
    }
}

} // namespace Vulqian::Engine::Graphics
