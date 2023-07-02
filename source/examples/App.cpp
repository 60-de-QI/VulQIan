// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "App.hpp"

#include <array>
#include <cassert>
#include <random>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

App::App() {
    this->load_entities();
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
        camera.set_perspective_projection(glm::radians(50.f), aspect, .1f, 1000.f);

        if (auto command_buffer = this->renderer.begin_frame()) {
            this->renderer.begin_SwapChain_RenderPass(command_buffer);
            render_system.render_entities(command_buffer, this->entities, camera, this->coordinator);
            this->renderer.end_SwapChain_RenderPass(command_buffer);
            this->renderer.end_frame();
        }
        this->physics_system->update();
    }
    vkDeviceWaitIdle(this->device.get_device());
}

void App::load_entities(void) {
    // ECS
    this->coordinator.init();
    this->coordinator.register_component<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>();
    this->coordinator.register_component<Vulqian::Engine::ECS::Components::Mesh>();

    this->physics_system = this->coordinator.register_system<Vulqian::Engine::ECS::Systems::Physics>();

    this->signature.set(this->coordinator.get_component_type<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>());
    this->signature.set(this->coordinator.get_component_type<Vulqian::Engine::ECS::Components::Mesh>());

    this->coordinator.set_system_signature<Vulqian::Engine::ECS::Systems::Physics>(signature);

    std::default_random_engine            generator;
    std::uniform_real_distribution<float> randPosition(-100.0f, 100.f);
    std::uniform_real_distribution<float> randRotation(0.0f, 3.0f);
    std::uniform_real_distribution<float> randScale(0.5f, 3.f);

    this->entities.reserve(Vulqian::Engine::ECS::MAX_ENTITIES);

    for (int i = 0; i != Vulqian::Engine::ECS::MAX_ENTITIES; i++) {
        float scale = randScale(generator);

        Vulqian::Engine::ECS::Entity entity = this->coordinator.create_entity();

        Vulqian::Engine::ECS::Components::Transform_TB_YXZ transform{};
        transform.scale = glm::vec3{scale, scale, scale};
        transform.rotation = glm::vec3{randRotation(generator), randRotation(generator), randRotation(generator)};
        transform.translation = glm::vec3{randPosition(generator), randPosition(generator), randPosition(generator)};

        Vulqian::Engine::ECS::Components::Mesh mesh{};
        mesh.model = Vulqian::Engine::Graphics::Model::create_model_from_file(this->device, "./conan-build/Debug/models/colored_cube.obj");

        this->coordinator.add_component(entity, Vulqian::Engine::ECS::Components::Transform_TB_YXZ{transform});
        this->coordinator.add_component(entity, Vulqian::Engine::ECS::Components::Mesh{mesh});
        this->entities.push_back(entity);
    }

}
