// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include <array>
#include <cassert>

#include "../../Exception/Exception.hpp"
#include "Renderer.hpp"

namespace Vulqian::Engine::Graphics {

Renderer::Renderer(Vulqian::Engine::Window& window, Vulqian::Engine::Graphics::Device& device) : window{window}, device{device} {
    this->recreate_swap_chain();
    this->create_command_buffers();
}

Renderer::~Renderer() {
    this->free_command_buffers();
}

void Renderer::recreate_swap_chain() {
    auto extent = this->window.get_extent();

    while (extent.width == 0 || extent.height == 0) {
        extent = this->window.get_extent();
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(this->device.get_device());

    if (this->swap_chain == nullptr) {
        this->swap_chain = std::make_unique<Vulqian::Engine::Graphics::SwapChain>(this->device, extent);
    } else {
        this->swap_chain = std::make_unique<Vulqian::Engine::Graphics::SwapChain>(this->device, extent, std::move(this->swap_chain));
        if (this->swap_chain->imageCount() != this->command_buffers.size()) {
            this->free_command_buffers();
            this->create_command_buffers();
        }
    }
    // we'll be right back to recreate swapchain
}

void Renderer::create_command_buffers() {
    this->command_buffers.resize(this->swap_chain->imageCount());

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = this->device.getCommandPool();
    alloc_info.commandBufferCount = static_cast<uint32_t>(this->command_buffers.size());

    if (vkAllocateCommandBuffers(this->device.get_device(), &alloc_info, this->command_buffers.data()) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_allocate("Command buffers");
    }
}

void Renderer::free_command_buffers(void) {
    vkFreeCommandBuffers(
        this->device.get_device(),
        this->device.getCommandPool(),
        static_cast<uint32_t>(this->command_buffers.size()),
        this->command_buffers.data());

    this->command_buffers.clear();
}

VkCommandBuffer Renderer::begin_frame(void) {
    assert(!this->is_frame_started && "can't call begin_Frame while already in progress");

    auto result = this->swap_chain->acquireNextImage(&this->current_image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        this->recreate_swap_chain();
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw Vulqian::Exception::failed_to_find("failed to acquire swap image chain image");
    }

    this->is_frame_started = true;

    auto command_buffer = get_current_commanBuffer();
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_setup("recording command buffer");
    }
    return command_buffer;
}

void Renderer::end_frame(void) {
    assert(is_frame_started && "Can't call end_frame while frame is not in progress");

    auto command_buffer = this->get_current_commanBuffer();

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_bind("and record command buffers");
    }

    auto result = this->swap_chain->submitCommandBuffers(&command_buffer, &this->current_image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || this->window.was_window_resized()) {
        this->window.reset_window_resized_flage();
        this->recreate_swap_chain();
    }
    if (result != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_open("swap chain image");
    }

    this->is_frame_started = false;
}

void Renderer::begin_SwapChain_RenderPass(VkCommandBuffer command_buffer) {
    assert(is_frame_started && "Can't call begin_SwapChain_RenderPass while frame is not in progress");
    assert(command_buffer == this->get_current_commanBuffer() && "Can't begin render pass on command buffer from different frame");

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = this->swap_chain->getRenderPass();
    render_pass_info.framebuffer = this->swap_chain->getFrameBuffer(this->current_image_index);

    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = this->swap_chain->getSwapChainExtent();

    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clear_values[1].depthStencil = {1.0f, static_cast<uint32_t>(0.0f)};
    render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(this->swap_chain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(this->swap_chain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, this->swap_chain->getSwapChainExtent()};
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

void Renderer::end_SwapChain_RenderPass(VkCommandBuffer command_buffer) const {
    assert(is_frame_started && "Can't call end_SwapChain_RenderPass while frame is not in progress");
    assert(command_buffer == this->get_current_commanBuffer() && "Can't end render pass on command buffer from different frame");

    vkCmdEndRenderPass(command_buffer);
}

} // namespace Vulqian::Engine::Graphics
