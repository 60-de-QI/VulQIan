// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "Manager.hpp"

#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <iostream>

#include "../../Exception/Exception.hpp"
#include "../Buffer/Buffer.hpp"
#include "../Device/Device.hpp"

namespace Vulqian::Engine::Graphics::Textures {
Manager::Manager(Device& device) : device{device} {
    createDefaultTextures();
}

std::shared_ptr<Texture> Manager::loadTexture(const std::string& filepath) {
    // Check cache first
    auto it = textureCache.find(filepath);
    if (it != textureCache.end()) {
        return it->second;
    }

    try {
        auto texture = std::make_shared<Texture>(device, filepath);
        textureCache[filepath] = texture;
        std::cout << "Loaded texture: " << filepath << std::endl;
        return texture;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load texture " << filepath << ": " << e.what() << std::endl;
        std::cerr << "Using missing texture fallback" << std::endl;
        return defaultMissing;
    }
}

void Manager::createDefaultTextures() {
    defaultWhite = Texture::createDefaultWhite(device);
    defaultMissing = Texture::createDefaultMissing(device);
    defaultNormal = Texture::createDefaultNormal(device);
    std::cout << "Created default textures" << std::endl;
}

}  // namespace Vulqian::Engine::Graphics::Textures
