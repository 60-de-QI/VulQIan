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

App::App() {
    this->load_world_objects();
}

void App::run() {
    Vulqian::Engine::Graphics::RenderSystem render_system{this->device, this->renderer.get_SwapChain_RenderPass()};

    while (!this->window.should_close()) {
        glfwPollEvents();

        if (auto command_buffer = this->renderer.begin_frame()) {
            this->renderer.begin_SwapChain_RenderPass(command_buffer);
            render_system.render_world_objects(command_buffer, this->world_objects);
            this->renderer.end_SwapChain_RenderPass(command_buffer);
            this->renderer.end_frame();
        }
    }
    vkDeviceWaitIdle(this->device.get_device());
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
