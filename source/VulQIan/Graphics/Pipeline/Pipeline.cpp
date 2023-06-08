// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "Pipeline.hpp"

#include <fstream>
#include <iostream>
#include <bit>

#include "../../Exception/Exception.hpp"

namespace Vulqian::Engine::Graphics {

Pipeline::Pipeline(
    Vulqian::Engine::Graphics::Device& device,
    const std::string& vert_filepath,
    const std::string& frag_filepath,
    const PipelineConstructInfo& config) : device(device) {
    this->create_graphics_pipeline(vert_filepath, frag_filepath, config);
}

Pipeline::~Pipeline() {
}

std::vector<char> Pipeline::read_file(const std::string& path) {
    std::ifstream file{path, std::ios::ate | std::ios::binary};

    if (!file.is_open()) {
        throw Vulqian::Exception::failed_to_open("file at " + path);
    }

    auto file_size = static_cast<size_t>(file.tellg());

    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);

    file.close();

    return buffer;
}

void Pipeline::create_graphics_pipeline(const std::string& vert_filepath, const std::string& frag_filepath, const PipelineConstructInfo& config) const {
    auto vert_code = this->read_file(vert_filepath);
    auto frag_code = this->read_file(frag_filepath);

    std::cout << "Vert size: " << vert_code.size() << std::endl;
    std::cout << "Frag size: " << frag_code.size() << std::endl;
}

void Pipeline::create_shader_module(const std::vector<char>& code, VkShaderModule* shader_mod) {
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = std::bit_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(this->device.device(), &create_info, nullptr, shader_mod) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_create("shader module");
    }
}

PipelineConstructInfo Pipeline::get_default_config(uint32_t width, uint32_t height) noexcept {
    PipelineConstructInfo default_conf{};

    return default_conf;
}

}  // namespace Vulqian::Engine::Graphics
