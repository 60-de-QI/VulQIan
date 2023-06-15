// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "../Device/Device.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>
#include <limits>
#include <iostream>
#include <tuple>
#include <array>

#include "../../Exception/Exception.hpp"

namespace Vulqian::Engine::Graphics {

class SwapChain {
   public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    SwapChain(Vulqian::Engine::Graphics::Device &deviceRef, VkExtent2D windowExtent);
    ~SwapChain();

    SwapChain(const SwapChain &) = delete;
    void operator=(const SwapChain &) = delete;

    VkFramebuffer getFrameBuffer(int index) noexcept { return swapChainFramebuffers[index]; }
    VkRenderPass getRenderPass() noexcept { return renderPass; }
    VkImageView getImageView(int index) noexcept { return swapChainImageViews[index]; }
    size_t imageCount() const noexcept { return swapChainImages.size(); }
    VkFormat getSwapChainImageFormat() const noexcept { return swapChainImageFormat; }
    VkExtent2D getSwapChainExtent() const noexcept { return swapChainExtent; }
    uint32_t width() const noexcept { return swapChainExtent.width; }
    uint32_t height() const noexcept { return swapChainExtent.height; }

    float extentAspectRatio() const noexcept {
        return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
    }
    VkFormat findDepthFormat();

    VkResult acquireNextImage(uint32_t *imageIndex);
    VkResult submitCommandBuffers(const VkCommandBuffer* buffers, const uint32_t* imageIndex);

   private:
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();

    // Helper functions
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) const;
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) const;
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;

    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkRenderPass renderPass;

    std::vector<VkImage> depthImages;
    std::vector<VkDeviceMemory> depthImageMemorys;
    std::vector<VkImageView> depthImageViews;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;

    Vulqian::Engine::Graphics::Device &device;
    VkExtent2D windowExtent;

    VkSwapchainKHR swapChain;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;
};

}  // namespace Vulqian::Graphics
