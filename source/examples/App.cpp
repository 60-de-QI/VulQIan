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
    this->create_pipeline();
}

App::~App() {
    vkDestroyPipelineLayout(this->device.get_device(), this->pipeline_layout, nullptr);
}

void App::run() {
    while (!this->window.should_close()) {
        glfwPollEvents();

        if (auto command_buffer = this->renderer.begin_frame()) {

            // We can implement render passes here now
            // begin offscreen shadow pass
            // render shadow casting objects
            // end offscreen shadow pass

            this->renderer.begin_SwapChain_RenderPass(command_buffer);
            this->render_world_objects(command_buffer);
            this->renderer.end_SwapChain_RenderPass(command_buffer);
            this->renderer.end_frame();
        }
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
    assert(this->pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");

    Vulqian::Engine::Graphics::PipelineConstructInfo pipeline_info{};
    Vulqian::Engine::Graphics::Pipeline::get_default_config(pipeline_info);

    pipeline_info.render_pass = this->renderer.get_SwapChain_RenderPass();
    pipeline_info.pipeline_layout = this->pipeline_layout;
    this->pipeline = std::make_unique<Vulqian::Engine::Graphics::Pipeline>(
        this->device,
        "./conan-build/Debug/Shaders/simple_shader.vert.spv",
        "./conan-build/Debug/Shaders/simple_shader.frag.spv",
        pipeline_info);
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
