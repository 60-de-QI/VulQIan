// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <memory>
#include <vector>

#include "../../ECS/ECS.hpp"
#include "../../Utils/Utils.hpp"
#include "../Camera/Camera.hpp"
#include "../Device/Device.hpp"
#include "../Frames/Frame.hpp"
#include "../Pipeline/Pipeline.hpp"
#include "Renderer.hpp"

namespace Vulqian::Engine::Graphics {

class RenderSystem {
   public:
    RenderSystem(Vulqian::Engine::Graphics::Device& device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout);
    ~RenderSystem();

    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;

    void render_entities(Vulqian::Engine::Graphics::Frames::Info& frame_info, const std::vector<Vulqian::Engine::ECS::Entity>& entities, Vulqian::Engine::ECS::Coordinator& coordinator);
    void render_single_entity(Vulqian::Engine::Graphics::Frames::Info& frame_info, Vulqian::Engine::ECS::Entity entity, Vulqian::Engine::ECS::Coordinator& coordinator);
    void render_opaque_entities_only(Vulqian::Engine::Graphics::Frames::Info& frame_info, const std::vector<Vulqian::Engine::ECS::Entity>& entities, Vulqian::Engine::ECS::Coordinator& coordinator);
    void render_transparent_entity(Vulqian::Engine::Graphics::Frames::Info& frame_info, Vulqian::Engine::ECS::Entity entity, Vulqian::Engine::ECS::Coordinator& coordinator);

   private:
    void create_pipeline_layout(VkDescriptorSetLayout global_set_layout);
    void create_pipeline(VkRenderPass render_pass);

    Vulqian::Engine::Graphics::Device& device;

    VkPipelineLayout pipeline_layout;

    std::unique_ptr<Vulqian::Engine::Graphics::Pipeline> pipeline;
};

}  // namespace Vulqian::Engine::Graphics
