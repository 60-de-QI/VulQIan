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
    glm::mat2 transform{2.f};
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
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

void RenderSystem::render_world_objects(VkCommandBuffer command_buffer, std::vector<Vulqian::Engine::Graphics::WorldObject>& world_objects) {
    this->pipeline->bind(command_buffer);

    for (auto& object : world_objects) {
        object.transform_2d.rotation = glm::mod(object.transform_2d.rotation + 0.01f, glm::two_pi<float>());
        SimplePushConstantData push{};
        push.offset = object.transform_2d.translation;
        push.color = object.color;

        push.transform = object.transform_2d.mat2();

        vkCmdPushConstants(
            command_buffer,
            this->pipeline_layout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push);
        object.model->bind(command_buffer);
        object.model->draw(command_buffer);
    }
}

} // namespace Vulqian::Engine::Graphics
