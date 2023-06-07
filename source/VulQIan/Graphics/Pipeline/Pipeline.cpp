// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "pipeline.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace Vulqian::Engine::Graphics {
Pipeline::Pipeline(const std::string& vert_filepath, const std::string& frag_filepath) {
    this->create_graphics_pipeline(vert_filepath, frag_filepath);
}

std::vector<char> Pipeline::read_file(const std::string& path) {
    std::ifstream file{path, std::ios::ate | std::ios::binary};

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file: " + path);
    }

    size_t file_size = static_cast<size_t>(file.tellg());

    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);

    file.close();

    return buffer;
}

void Pipeline::create_graphics_pipeline(const std::string& vert_filepath, const std::string& frag_filepath) {
    auto vert_code = this->read_file(vert_filepath);
    auto frag_code = this->read_file(frag_filepath);

    std::cout << "Vert size: " << vert_code.size() << std::endl;
    std::cout << "Frag size: " << frag_code.size() << std::endl;
}
}  // namespace Vulqian::Engine::Graphics
