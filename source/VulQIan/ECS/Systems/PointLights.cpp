// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "PointLights.hpp"

#include "../../Exception/Exception.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/ext/matrix_transform.hpp>
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
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(LightsPushConstants);

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
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
        "./conan-build/Shaders/point_light.vert.spv",
        "./conan-build/Shaders/point_light.frag.spv",
        pipelineConfig);
}

void PointLights::render(Vulqian::Engine::Graphics::Frames::Info&         frameInfo,
            Vulqian::Engine::ECS::Coordinator&               coordinator,
            const std::vector<Vulqian::Engine::ECS::Entity>& entities) {
    // Bind the pipeline
    this->pipeline->bind(frameInfo.command_buffer);

    // Bind global descriptor sets
    vkCmdBindDescriptorSets(
        frameInfo.command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout,
        0,
        1,
        &frameInfo.global_descriptor_set,
        0,
        nullptr);

    // Iterate through all entities to find and render those with PointLight components
    for (const auto& entity : entities) {
        // Check if this entity has a PointLight component
        if (coordinator.has_component<Vulqian::Engine::ECS::Components::PointLight>(entity)) {
            // Get the transform and point light components
            auto& transform = coordinator.get_component<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>(entity);
            auto& pointLight = coordinator.get_component<Vulqian::Engine::ECS::Components::PointLight>(entity);

            // Prepare push constants for this light
            LightsPushConstants push{};
            push.position = glm::vec4(transform.translation, 1.f);

            // Set color - using white light with intensity from the component
            // You can modify this to use a color component if you add one later
            push.colour = glm::vec4(1.0f, 1.0f, 1.0f, pointLight.lightIntensity);

            // Use the scale.x as radius (you might want to add a specific radius field to PointLight)
            push.radius = transform.scale.x;

            // Push constants to shader
            vkCmdPushConstants(
                frameInfo.command_buffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(LightsPushConstants),
                &push);

            // Draw this light (6 vertices for a quad)
            vkCmdDraw(frameInfo.command_buffer, 6, 1, 0, 0);
        }
    }
}

void PointLights::update(Vulqian::Engine::Graphics::Frames::Info&         frameInfo,
                         Vulqian::Engine::Graphics::Frames::GlobalUbo&    ubo,
                         Vulqian::Engine::ECS::Coordinator&               coordinator,
                         const std::vector<Vulqian::Engine::ECS::Entity>& entities) {
    auto rotateLight{glm::rotate(glm::mat4(1.f), 0.5f * frameInfo.frame_time, {0.f, -1.f, 0.f})};
    int  lightIndex{0};

    // Iterate through all entities to find those with PointLight components
    for (const auto& entity : entities) {
        // Check if this entity has a PointLight component
        if (coordinator.has_component<Vulqian::Engine::ECS::Components::PointLight>(entity)) {
            assert(lightIndex < Vulqian::Engine::Graphics::Frames::MAX_LIGHTS && "Point lights exceed maximum specified");

            // Get the transform and point light components
            auto& transform = coordinator.get_component<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>(entity);
            auto& pointLight = coordinator.get_component<Vulqian::Engine::ECS::Components::PointLight>(entity);

            // Update light position with rotation
            // transform.translation = glm::vec3(rotateLight * glm::vec4(transform.translation, 1.f));

            // Copy light data to UBO
            ubo.pointLights[lightIndex].position = glm::vec4(transform.translation, 1.f);

            // Set color - using white light with the intensity from the component
            // You can modify this to use a color component if you add one later
            ubo.pointLights[lightIndex].color = glm::vec4(1.0f, 1.0f, 1.0f, pointLight.lightIntensity);

            ++lightIndex;
        }
    }

    ubo.numLights = lightIndex;
}

}  // namespace Vulqian::Engine::ECS::Systems
