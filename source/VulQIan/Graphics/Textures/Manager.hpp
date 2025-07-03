// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <array>
#include <memory>
#include <string>
#include <unordered_map>

#include "../../Exception/Exception.hpp"
#include "../Buffer/Buffer.hpp"
#include "../Device/Device.hpp"
#include "Texture.hpp"

namespace Vulqian::Engine::Graphics::Textures {

class Manager {
   public:
    explicit Manager(Device& device);
    ~Manager() = default;

    // Load texture with automatic fallback on failure
    std::shared_ptr<Vulqian::Engine::Graphics::Textures::Texture> loadTexture(const std::string& filepath);

    // Get default textures
    std::shared_ptr<Vulqian::Engine::Graphics::Textures::Texture> getDefaultWhite() const noexcept { return defaultWhite; }
    std::shared_ptr<Vulqian::Engine::Graphics::Textures::Texture> getDefaultMissing() const noexcept { return defaultMissing; }
    std::shared_ptr<Vulqian::Engine::Graphics::Textures::Texture> getDefaultNormal() const noexcept { return defaultNormal; }

   private:
    Device& device;

    // Cache for loaded textures
    std::unordered_map<std::string, std::shared_ptr<Vulqian::Engine::Graphics::Textures::Texture>> textureCache;

    // Default textures
    std::shared_ptr<Vulqian::Engine::Graphics::Textures::Texture> defaultWhite;
    std::shared_ptr<Vulqian::Engine::Graphics::Textures::Texture> defaultMissing;  // Pink/magenta for missing textures
    std::shared_ptr<Vulqian::Engine::Graphics::Textures::Texture> defaultNormal;   // Flat normal map

    void createDefaultTextures();
};

}  // namespace Vulqian::Engine::Graphics::Textures
