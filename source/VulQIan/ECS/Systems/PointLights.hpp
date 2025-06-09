// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "../../Graphics/Camera/Camera.hpp"
#include "../../Graphics/Device/Device.hpp"
#include "../../Graphics/Frames/Frame.hpp"
#include "../../Graphics/Pipeline/Pipeline.hpp"
// #include "lve_game_object.hpp"

// std
#include <memory>
#include <vector>

namespace Vulqian::Engine::ECS::Systems {
class PointLights {
   public:
    PointLights(Vulqian::Engine::Graphics::Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~PointLights();

    PointLights(const PointLights &) = delete;
    PointLights &operator=(const PointLights &) = delete;

    void render(Vulqian::Engine::Graphics::Frames::Info& frameInfo);

   private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Vulqian::Engine::Graphics::Device &device;

    std::unique_ptr<Vulqian::Engine::Graphics::Pipeline> pipeline;
    VkPipelineLayout             pipelineLayout;
};
}  // namespace Vulqian::Engine::ECS::Systems
