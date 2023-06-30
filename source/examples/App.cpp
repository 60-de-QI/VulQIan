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

    camera.set_view_target(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    auto viewer_object = Vulqian::Engine::Graphics::WorldObject::create_game_object();

    Vulqian::Engine::Input::KeyboardMovementController camera_controller{};

    auto current_time = std::chrono::high_resolution_clock::now();

    while (!this->window.should_close()) {
        glfwPollEvents();

        auto  new_time = std::chrono::high_resolution_clock::now();
        float frame_time = std::chrono::duration<float, std::chrono::seconds::period>(new_time - current_time).count();
        current_time = new_time;

        camera_controller.move_in_plane_xz(this->window.get_window(), frame_time, viewer_object);
        camera_controller.update_camera_orientation(this->window.get_window(), frame_time, viewer_object);

        camera.set_view_YXZ(viewer_object.transform.translation, viewer_object.transform.rotation);

        float aspect = this->renderer.get_aspect_ratio();
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
    Vulqian::Engine::Graphics::Model::Data model_data{};

    model_data.vertices = {
        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

        // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    };

    model_data.indices = {0, 1, 2, 0, 3, 1, 4, 5, 6, 4, 7, 5, 8, 9, 10, 8, 11, 9,
                          12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};

    return std::make_unique<Vulqian::Engine::Graphics::Model>(device, model_data);
}

std::unique_ptr<Vulqian::Engine::Graphics::Model> createPyramidModel(Vulqian::Engine::Graphics::Device& device, glm::vec3 offset) {

    Vulqian::Engine::Graphics::Model::Data model_data{};

    model_data.vertices = {

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

    for (auto& v : model_data.vertices) {
        v.position += offset;
    }

    return std::make_unique<Vulqian::Engine::Graphics::Model>(device, model_data);
}

void App::load_world_objects(void) {
    std::shared_ptr<Vulqian::Engine::Graphics::Model> lveModel = createCubeModel(device, {.0f, .0f, .0f});

    auto cube = Vulqian::Engine::Graphics::WorldObject::create_game_object();
    cube.model = lveModel;
    cube.transform.translation = {.0f, .0f, 2.5f};
    cube.transform.scale = {.5f, .5f, .5f};

    this->world_objects.push_back(std::move(cube));
}
