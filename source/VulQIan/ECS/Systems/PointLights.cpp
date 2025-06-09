// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "PointLights.hpp"

#include "../../Exception/Exception.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace Vulqian::Engine::ECS::Systems {

PointLights::PointLights(Vulqian::Engine::Graphics::Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
    : device{device} {
    createPipelineLayout(globalSetLayout);
    createPipeline(renderPass);
}

PointLights::~PointLights() {
    vkDestroyPipelineLayout(this->device.get_device(), pipelineLayout, nullptr);
}

void PointLights::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    // VkPushConstantRange pushConstantRange{};
    // pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    // pushConstantRange.offset = 0;
    // pushConstantRange.size = sizeof(SimplePushConstantData);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(this->device.get_device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
        VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_create("pipeline layout in point lights system");
    }
}

void PointLights::createPipeline(VkRenderPass renderPass) {
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    Vulqian::Engine::Graphics::PipelineConstructInfo pipelineConfig{};
    Vulqian::Engine::Graphics::Pipeline::get_default_config(pipelineConfig);
    pipelineConfig.attribute_descriptions.clear();
    pipelineConfig.binding_descriptions.clear();
    pipelineConfig.render_pass = renderPass;
    pipelineConfig.pipeline_layout = pipelineLayout;
    this->pipeline = std::make_unique<Vulqian::Engine::Graphics::Pipeline>(
        this->device,
        "/conan-build/Shaders/point_light.vert.spv",
        "/conan-build/Shaders/point_light.frag.spv",
        pipelineConfig);
}

void PointLights::render(Vulqian::Engine::Graphics::Frames::Info& frameInfo) {
    this->pipeline->bind(frameInfo.command_buffer);

    vkCmdBindDescriptorSets(
        frameInfo.command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        0,
        1,
        &frameInfo.global_descriptor_set,
        0,
        nullptr);

    vkCmdDraw(frameInfo.command_buffer, 6, 1, 0, 0);
}

}  // namespace Vulqian::Engine::ECS::Systems
