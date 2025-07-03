// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "../../ECS/ECS.hpp"
#include "../../Utils/Utils.hpp"
#include "../Camera/Camera.hpp"
#include "../Descriptors/Descriptors.hpp"
#include "../Device/Device.hpp"
#include "../Frames/Frame.hpp"
#include "../Pipeline/Pipeline.hpp"
#include "../Textures/Manager.hpp"
#include "Renderer.hpp"

namespace Vulqian::Engine::Graphics {

// Custom hash function for VkDescriptorImageInfo
struct DescriptorImageInfoHash {
    std::size_t operator()(const VkDescriptorImageInfo& info) const noexcept {
        // Combine hashes of the pointers and values
        std::size_t h1 = std::hash<VkSampler>{}(info.sampler);
        std::size_t h2 = std::hash<VkImageView>{}(info.imageView);
        std::size_t h3 = std::hash<int>{}(static_cast<int>(info.imageLayout));

        // Simple hash combination
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

// Custom equality function for VkDescriptorImageInfo
struct DescriptorImageInfoEqual {
    bool operator()(const VkDescriptorImageInfo& lhs, const VkDescriptorImageInfo& rhs) const noexcept {
        return lhs.sampler == rhs.sampler &&
               lhs.imageView == rhs.imageView &&
               lhs.imageLayout == rhs.imageLayout;
    }
};

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

    void setTextureManager(Vulqian::Engine::Graphics::Textures::Manager* manager) noexcept { this->textureManager = manager; }

   private:
    void create_pipeline_layout(const std::vector<VkDescriptorSetLayout>& descriptor_set_layouts);  // Updated signature
    void create_pipeline(VkRenderPass render_pass);

    Vulqian::Engine::Graphics::Device& device;

    VkPipelineLayout pipeline_layout;

    std::unique_ptr<Vulqian::Engine::Graphics::Pipeline>                                                          pipeline;
    std::unique_ptr<Vulqian::Engine::Graphics::Descriptors::DescriptorSetLayout>                                  textureSetLayout;
    std::unique_ptr<Vulqian::Engine::Graphics::Descriptors::DescriptorPool>                                       texturePool;
    std::unordered_map<VkDescriptorImageInfo, VkDescriptorSet, DescriptorImageInfoHash, DescriptorImageInfoEqual> textureDescriptorCache;  // FIXED LINE
    Vulqian::Engine::Graphics::Textures::Manager*                                                                 textureManager{nullptr};

    VkDescriptorSet getOrCreateTextureDescriptorSet(const VkDescriptorImageInfo& imageInfo);
};

}  // namespace Vulqian::Engine::Graphics
