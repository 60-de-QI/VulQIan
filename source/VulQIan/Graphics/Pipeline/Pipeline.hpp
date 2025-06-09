// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <string>
#include <vector>

#include "../Device/Device.hpp"

namespace Vulqian::Engine::Graphics {

struct PipelineConstructInfo {
    PipelineConstructInfo() = default;
    PipelineConstructInfo(const PipelineConstructInfo&) = delete;
    PipelineConstructInfo& operator=(const PipelineConstructInfo&) = delete;

    std::vector<VkVertexInputBindingDescription>   binding_descriptions{};
    std::vector<VkVertexInputAttributeDescription> attribute_descriptions{};
    VkPipelineViewportStateCreateInfo              viewport_info;
    VkPipelineInputAssemblyStateCreateInfo         input_assembly_info;
    VkPipelineRasterizationStateCreateInfo         rasterization_info;
    VkPipelineMultisampleStateCreateInfo           multisample_info;
    VkPipelineColorBlendAttachmentState            color_blend_attachment;
    VkPipelineColorBlendStateCreateInfo            color_blend_info;
    VkPipelineDepthStencilStateCreateInfo          depth_stencil_info;
    std::vector<VkDynamicState>                    dynamic_state_enables;
    VkPipelineDynamicStateCreateInfo               dynamic_state_info;
    VkPipelineLayout                               pipeline_layout = nullptr;
    VkRenderPass                                   render_pass = nullptr;
    uint32_t                                       subpass = 0;
};

class Pipeline {
   public:
    Pipeline(Vulqian::Engine::Graphics::Device& device, const std::string& vert_filepath, const std::string& frag_filepath, const PipelineConstructInfo& config);
    ~Pipeline();

    // We do this to respect RAII and avoid duplicating pointers to our Pipeline
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    void        bind(VkCommandBuffer command_buffer);
    static void get_default_config(PipelineConstructInfo& default_conf) noexcept;

   private:
    void create_graphics_pipeline(const std::string& vert_filepath, const std::string& frag_filepath, const PipelineConstructInfo& config);
    void create_shader_module(const std::vector<char>& code, VkShaderModule* shader_mod) const;

    static std::vector<char> read_file(const std::string& path);
    Device&                  device;
    VkPipeline               graphics_pipeline;
    VkShaderModule           vert_module;
    VkShaderModule           frag_module;
};
}  // namespace Vulqian::Engine::Graphics
