// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "App.hpp"

#include <array>

App::App() {
    this->create_pipeline_layout();
    this->create_pipeline();
    this->create_command_buffers();
}

App::~App() {
    vkDestroyPipelineLayout(this->device.get_device(), this->pipeline_layout, nullptr);
}

void App::run() {
    while (!this->window.should_close()) {
        glfwPollEvents();
        this->draw_frame();
    }
    vkDeviceWaitIdle(this->device.get_device());
}

void App::create_pipeline_layout() {
    VkPipelineLayoutCreateInfo pipeline_create_info{};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_create_info.setLayoutCount = 0;
    pipeline_create_info.pSetLayouts = nullptr;
    pipeline_create_info.pushConstantRangeCount = 0;
    pipeline_create_info.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(this->device.get_device(), &pipeline_create_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_create("pipeline layout");
    }
}

void App::create_pipeline() {
    Vulqian::Engine::Graphics::PipelineConstructInfo pipeline_info{};
    Vulqian::Engine::Graphics::Pipeline::get_default_config(pipeline_info, this->swap_chain.width(), this->swap_chain.height());

    pipeline_info.render_pass = this->swap_chain.getRenderPass();
    pipeline_info.pipeline_layout = this->pipeline_layout;
    this->pipeline = std::make_unique<Vulqian::Engine::Graphics::Pipeline>(
        this->device,
        "./conan-build/Debug/Shaders/simple_shader.vert.spv",
        "./conan-build/Debug/Shaders/simple_shader.frag.spv",
        pipeline_info);
}

void App::create_command_buffers() {
    this->command_buffers.resize(this->swap_chain.imageCount());

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = this->device.getCommandPool();
    alloc_info.commandBufferCount = static_cast<uint32_t>(this->command_buffers.size());

    if (vkAllocateCommandBuffers(this->device.get_device(), &alloc_info, this->command_buffers.data()) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_allocate("Command buffers");
    }

    for (int i = 0; i < this->command_buffers.size(); i++) {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(this->command_buffers[i], &begin_info) != VK_SUCCESS) {
            throw Vulqian::Exception::failed_to_setup("recording command buffer");
        }

        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = this->swap_chain.getRenderPass();
        render_pass_info.framebuffer = this->swap_chain.getFrameBuffer(i);

        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = this->swap_chain.getSwapChainExtent();

        std::array<VkClearValue, 2> clear_values{};
        clear_values[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clear_values[1].depthStencil = {1.0f, static_cast<uint32_t>(0.0f)};
        render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
        render_pass_info.pClearValues = clear_values.data();

        vkCmdBeginRenderPass(this->command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        this->pipeline->bind(this->command_buffers[i]);
        vkCmdDraw(this->command_buffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(this->command_buffers[i]);
        if (vkEndCommandBuffer(this->command_buffers[i]) != VK_SUCCESS) {
            throw Vulqian::Exception::failed_to_bind("and record command buffers");
        }
    }
}

void App::draw_frame() {
    uint32_t image_index;
    auto result = this->swap_chain.acquireNextImage(&image_index);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw Vulqian::Exception::failed_to_find("failed to acquire swap image chain image");
    }

    result = this->swap_chain.submitCommandBuffers(&command_buffers[image_index], &image_index);
    if (result != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_open("swap chain image");
    }
}
