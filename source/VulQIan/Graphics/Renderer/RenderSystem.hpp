// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "../../ECS/ECS.hpp"
#include "../Camera/Camera.hpp"
#include "../Device/Device.hpp"
#include "../Pipeline/Pipeline.hpp"
#include "../WorldObject/WorldObject.hpp"
#include "Renderer.hpp"

#include <memory>
#include <vector>

namespace Vulqian::Engine::Graphics {

class RenderSystem {
  public:
    RenderSystem(Vulqian::Engine::Graphics::Device& device, VkRenderPass render_pass);
    ~RenderSystem();

    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;

    void render_world_objects(VkCommandBuffer command_buffer, std::vector<Vulqian::Engine::Graphics::WorldObject>& world_objects, const Vulqian::Engine::Graphics::Camera& camera);
    void render_world_objects(VkCommandBuffer command_buffer, std::vector<Vulqian::Engine::ECS::Entity>& entities, const Vulqian::Engine::Graphics::Camera& camera);

  private:
    void create_pipeline_layout(void);
    void create_pipeline(VkRenderPass render_pass);

    Vulqian::Engine::Graphics::Device& device;

    VkPipelineLayout pipeline_layout;

    std::unique_ptr<Vulqian::Engine::Graphics::Pipeline> pipeline;
};

} // namespace Vulqian::Engine::Graphics
