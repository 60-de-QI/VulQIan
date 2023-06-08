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
};

class Pipeline {
   public:
    Pipeline(Vulqian::Engine::Graphics::Device& device, const std::string& vert_filepath, const std::string& frag_filepath, const PipelineConstructInfo& config);
    ~Pipeline();

    // We do this to respect RAII and avoid duplicating pointers to our Pipeline
    Pipeline(const Pipeline&) = delete;
    void operator=(const Pipeline&) = delete;

    static PipelineConstructInfo get_default_config(uint32_t width, uint32_t height) noexcept;

   private:
    void create_graphics_pipeline(const std::string& vert_filepath, const std::string& frag_filepath, const PipelineConstructInfo& config) const;
    void create_shader_module(const std::vector<char>& code, VkShaderModule* shader_mod);

    static std::vector<char> read_file(const std::string& path);
    Device& device;
    VkPipeline graphics_pipeline;
    VkShaderModule vert_module;
    VkShaderModule frag_module;
};
}  // namespace Vulqian::Engine::Graphics
