// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "App.hpp"

#include <array>
#include <cassert>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct SimplePushConstantData {
    glm::mat2 transform{2.f};
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};

App::App() {
    this->load_world_objects();
    this->create_pipeline_layout();
    this->recreate_swap_chain();
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
    VkPushConstantRange constant_range{};
    constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    constant_range.offset = 0;
    constant_range.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipeline_create_info{};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_create_info.setLayoutCount = 0;
    pipeline_create_info.pSetLayouts = nullptr;
    pipeline_create_info.pushConstantRangeCount = 1;
    pipeline_create_info.pPushConstantRanges = &constant_range;

    if (vkCreatePipelineLayout(this->device.get_device(), &pipeline_create_info, nullptr, &pipeline_layout) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_create("pipeline layout");
    }
}

void App::create_pipeline() {
    assert(this->swap_chain != nullptr && "Cannot create pipeline before swap chain");
    assert(this->pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");

    Vulqian::Engine::Graphics::PipelineConstructInfo pipeline_info{};
    Vulqian::Engine::Graphics::Pipeline::get_default_config(pipeline_info);

    pipeline_info.render_pass = this->swap_chain->getRenderPass();
    pipeline_info.pipeline_layout = this->pipeline_layout;
    this->pipeline = std::make_unique<Vulqian::Engine::Graphics::Pipeline>(
        this->device,
        "./conan-build/Debug/Shaders/simple_shader.vert.spv",
        "./conan-build/Debug/Shaders/simple_shader.frag.spv",
        pipeline_info);
}

void App::recreate_swap_chain() {
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

    this->create_pipeline();
}

void App::create_command_buffers() {
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

void App::free_command_buffers(void) {
    vkFreeCommandBuffers(
        this->device.get_device(),
        this->device.getCommandPool(),
        static_cast<uint32_t>(this->command_buffers.size()),
        this->command_buffers.data());

    this->command_buffers.clear();
}

void App::record_command_buffer(int image_index) {
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(this->command_buffers[image_index], &begin_info) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_setup("recording command buffer");
    }

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = this->swap_chain->getRenderPass();
    render_pass_info.framebuffer = this->swap_chain->getFrameBuffer(image_index);

    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = this->swap_chain->getSwapChainExtent();

    std::array<VkClearValue, 2> clear_values{};
    clear_values[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clear_values[1].depthStencil = {1.0f, static_cast<uint32_t>(0.0f)};
    render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(this->command_buffers[image_index], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(this->swap_chain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(this->swap_chain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, this->swap_chain->getSwapChainExtent()};
    vkCmdSetViewport(this->command_buffers[image_index], 0, 1, &viewport);
    vkCmdSetScissor(this->command_buffers[image_index], 0, 1, &scissor);

    this->render_world_objects(this->command_buffers[image_index]);

    vkCmdEndRenderPass(this->command_buffers[image_index]);
    if (vkEndCommandBuffer(this->command_buffers[image_index]) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_bind("and record command buffers");
    }
}

void App::render_world_objects(VkCommandBuffer command_buffer) {
    this->pipeline->bind(command_buffer);

    for (auto& object : this->world_objects) {
        object.transform_2d.rotation = glm::mod(object.transform_2d.rotation + 0.0001f, glm::two_pi<float>());
        SimplePushConstantData push{};
        push.offset = object.transform_2d.translation;
        push.color = object.color;

        push.transform = object.transform_2d.mat2();

        vkCmdPushConstants(
            command_buffer,
            this->pipeline_layout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push);
        object.model->bind(command_buffer);
        object.model->draw(command_buffer);
    }
}

void App::draw_frame() {
    uint32_t image_index;
    auto result = this->swap_chain->acquireNextImage(&image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        this->recreate_swap_chain();
        return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw Vulqian::Exception::failed_to_find("failed to acquire swap image chain image");
    }

    this->record_command_buffer(image_index);
    result = this->swap_chain->submitCommandBuffers(&command_buffers[image_index], &image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || this->window.was_window_resized()) {
        this->window.reset_window_resized_flage();
        this->recreate_swap_chain();
        return;
    }
    if (result != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_open("swap chain image");
    }
}

void App::generate_sierpinski_triangle(std::vector<Vulqian::Engine::Graphics::Model::Vertex>& vertices, int level, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c) {
    if (level == 0) {
        // Add the vertices of the triangle to the vector
        vertices.push_back(Vulqian::Engine::Graphics::Model::Vertex(a));
        vertices.push_back(Vulqian::Engine::Graphics::Model::Vertex(b));
        vertices.push_back(Vulqian::Engine::Graphics::Model::Vertex(c));
    } else {
        // Calculate the midpoints of the sides
        glm::vec2 ab = (a + b) * 0.5f;
        glm::vec2 ac = (a + c) * 0.5f;
        glm::vec2 bc = (b + c) * 0.5f;

        // Recursively generate the smaller triangles
        generate_sierpinski_triangle(vertices, level - 1, a, ab, ac);
        generate_sierpinski_triangle(vertices, level - 1, ab, b, bc);
        generate_sierpinski_triangle(vertices, level - 1, ac, bc, c);
    }
}

void App::load_world_objects(void) {
    std::vector<Vulqian::Engine::Graphics::Model::Vertex> vertices{
        {{0.0f, -0.5f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
    // generate_sierpinski_triangle(vertices, 6, {0.0f, -0.5f}, {0.5f, 0.5f}, {-0.5f, 0.5f});

    auto model = std::make_shared<Vulqian::Engine::Graphics::Model>(this->device, vertices);

    auto triangle = Vulqian::Engine::Graphics::WorldObject::create_game_object();
    triangle.model = model;
    triangle.color = {.1f, .8f, .1f};
    triangle.transform_2d.translation.x = .2f;
    triangle.transform_2d.scale = {2.f, .5f};
    triangle.transform_2d.rotation = .25f * glm::two_pi<float>();    

    this->world_objects.push_back(std::move(triangle));
}
