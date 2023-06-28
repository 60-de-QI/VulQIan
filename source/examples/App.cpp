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
    Vulqian::Engine::Graphics::Camera       camera{};
    // camera.set_view_direction(glm::vec3{0.f}, glm::vec3{0.5f, 0.f, 1.f});
    camera.set_view_target(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    while (!this->window.should_close()) {
        glfwPollEvents();

        float aspect = this->renderer.get_aspect_ratio();
        // camera.set_orthographic_projection(-aspect, aspect, -1, 1, -1, 1);
        camera.set_perspective_projection(glm::radians(50.f), aspect, .1f, 10.f);

        if (auto command_buffer = this->renderer.begin_frame()) {
            this->renderer.begin_SwapChain_RenderPass(command_buffer);
            render_system.render_world_objects(command_buffer, this->world_objects, camera);
            this->renderer.end_SwapChain_RenderPass(command_buffer);
            this->renderer.end_frame();
        }
    }
    vkDeviceWaitIdle(this->device.get_device());
}

// temporary helper function, creates a 1x1x1 cube centered at offset
std::unique_ptr<Vulqian::Engine::Graphics::Model> createCubeModel(Vulqian::Engine::Graphics::Device& device, glm::vec3 offset) {
    std::vector<Vulqian::Engine::Graphics::Model::Vertex> vertices{

        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

        // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

    };
    for (auto& v : vertices) {
        v.position += offset;
    }
    return std::make_unique<Vulqian::Engine::Graphics::Model>(device, vertices);
}

std::unique_ptr<Vulqian::Engine::Graphics::Model> createPyramidModel(Vulqian::Engine::Graphics::Device& device, glm::vec3 offset) {
    std::vector<Vulqian::Engine::Graphics::Model::Vertex> vertices{

        // base (white)
        {{-0.5f, -0.5f, -0.5f}, {0.9f, 0.9f, 0.9f}},
        {{0.5f, -0.5f, -0.5f}, {0.9f, 0.9f, 0.9f}},
        {{-0.5f, -0.5f, 0.5f}, {0.9f, 0.9f, 0.9f}},
        {{0.5f, -0.5f, -0.5f}, {0.9f, 0.9f, 0.9f}},
        {{0.5f, -0.5f, 0.5f}, {0.9f, 0.9f, 0.9f}},
        {{-0.5f, -0.5f, 0.5f}, {0.9f, 0.9f, 0.9f}},

        // front face (yellow)
        {{0.0f, 0.5f, 0.0f}, {0.8f, 0.8f, 0.1f}},
        {{-0.5f, -0.5f, 0.5f}, {0.8f, 0.8f, 0.1f}},
        {{0.5f, -0.5f, 0.5f}, {0.8f, 0.8f, 0.1f}},

        // left face (orange)
        {{0.0f, 0.5f, 0.0f}, {0.9f, 0.6f, 0.1f}},
        {{-0.5f, -0.5f, -0.5f}, {0.9f, 0.6f, 0.1f}},
        {{-0.5f, -0.5f, 0.5f}, {0.9f, 0.6f, 0.1f}},

        // right face (red)
        {{0.0f, 0.5f, 0.0f}, {0.8f, 0.1f, 0.1f}},
        {{0.5f, -0.5f, -0.5f}, {0.8f, 0.1f, 0.1f}},
        {{0.5f, -0.5f, 0.5f}, {0.8f, 0.1f, 0.1f}},

        // back face (blue)
        {{0.0f, 0.5f, 0.0f}, {0.1f, 0.1f, 0.8f}},
        {{-0.5f, -0.5f, -0.5f}, {0.1f, 0.1f, 0.8f}},
        {{0.5f, -0.5f, -0.5f}, {0.1f, 0.1f, 0.8f}},
    };

    for (auto& v : vertices) {
        v.position += offset;
    }

    return std::make_unique<Vulqian::Engine::Graphics::Model>(device, vertices);
}

void App::load_world_objects(void) {
    std::shared_ptr<Vulqian::Engine::Graphics::Model> lveModel = createCubeModel(device, {.0f, .0f, .0f});

    auto cube = Vulqian::Engine::Graphics::WorldObject::create_game_object();
    cube.model = lveModel;
    cube.transform.translation = {.0f, .0f, 2.5f};
    cube.transform.scale = {.5f, .5f, .5f};

    this->world_objects.push_back(std::move(cube));
}
