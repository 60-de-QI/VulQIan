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

namespace Vulqian::Engine::Graphics::Textures {

class Texture {
   public:
    // Main constructor from file path
    Texture(Device& device, const std::string& texturePath);

    // Factory methods for default textures
    static std::unique_ptr<Texture> createDefaultWhite(Device& device);
    static std::unique_ptr<Texture> createDefaultMissing(Device& device);
    static std::unique_ptr<Texture> createDefaultNormal(Device& device);

    ~Texture();

    // No copy, move only
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    // Accessors
    VkImage     getImage() const { return textureImage; }
    VkImageView getImageView() const { return textureImageView; }
    VkSampler   getSampler() const { return textureSampler; }
    uint32_t    getWidth() const { return width; }
    uint32_t    getHeight() const { return height; }
    uint32_t    getMipLevels() const { return mipLevels; }

    // Convenience method for descriptor binding
    VkDescriptorImageInfo getDescriptorInfo() const {
        return {textureSampler, textureImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
    }

   private:
    // Private constructor for procedural textures
    explicit Texture(Device& device) : device{device} {}

    void createTextureImage(const std::string& texturePath);
    void createTextureImageView();
    void createTextureSampler();
    void createProceduralTexture(uint32_t width, uint32_t height, const std::array<uint8_t, 4>& color);

    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    Device& device;

    VkImage        textureImage{VK_NULL_HANDLE};
    VkDeviceMemory textureImageMemory{VK_NULL_HANDLE};
    VkImageView    textureImageView{VK_NULL_HANDLE};
    VkSampler      textureSampler{VK_NULL_HANDLE};

    uint32_t mipLevels{1};
    uint32_t width{0};
    uint32_t height{0};
};

}  // namespace Vulqian::Engine::Graphics
