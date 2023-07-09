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
    this->load_systems();
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
    }
    vkDeviceWaitIdle(this->device.get_device());
}

void App::load_vase(void) {
    Vulqian::Engine::ECS::Entity smooth_vase = this->coordinator.create_entity();

    Vulqian::Engine::ECS::Components::Transform_TB_YXZ transform{};

    transform.scale = glm::vec3{3.f, 1.5f, 3.f};
    transform.translation = glm::vec3{-.5f, .5f, 2.5f};

    Vulqian::Engine::ECS::Components::Mesh mesh{};
    mesh.model = Vulqian::Engine::Graphics::Model::create_model_from_file(this->device, Vulqian::Engine::Utils::smooth_vase);

    this->coordinator.add_component(smooth_vase, Vulqian::Engine::ECS::Components::Transform_TB_YXZ{transform});
    this->coordinator.add_component(smooth_vase, Vulqian::Engine::ECS::Components::Mesh{mesh});
    this->entities.push_back(smooth_vase);

    // flat vase
    Vulqian::Engine::ECS::Entity flat_vase = this->coordinator.create_entity();

    Vulqian::Engine::ECS::Components::Transform_TB_YXZ transform_flat{};

    transform_flat.scale = {3.f, 1.5f, 3.f};
    transform_flat.translation = {.5f, .5f, 2.5f};

    Vulqian::Engine::ECS::Components::Mesh flat_mesh{};
    flat_mesh.model = Vulqian::Engine::Graphics::Model::create_model_from_file(this->device, Vulqian::Engine::Utils::flat_vase);

    this->coordinator.add_component(flat_vase, Vulqian::Engine::ECS::Components::Transform_TB_YXZ{transform_flat});
    this->coordinator.add_component(flat_vase, Vulqian::Engine::ECS::Components::Mesh{flat_mesh});
    this->entities.push_back(flat_vase);
}

void App::load_systems(void) {
    this->physics_system = this->coordinator.register_system<Vulqian::Engine::ECS::Systems::Physics>();
    this->coordinator.set_system_signature<Vulqian::Engine::ECS::Systems::Physics>(signature);
}

void App::load_entities(void) {
    // ECS
    this->coordinator.init();
    this->coordinator.register_component<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>();
    this->coordinator.register_component<Vulqian::Engine::ECS::Components::Mesh>();

    this->signature.set(this->coordinator.get_component_type<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>());
    this->signature.set(this->coordinator.get_component_type<Vulqian::Engine::ECS::Components::Mesh>());

    std::default_random_engine            generator;
    std::uniform_real_distribution<float> randPosition(-100.0f, 100.0f);
    std::uniform_real_distribution<float> randRotation(.0f, 3.0f);
    std::uniform_real_distribution<float> randScale(0.5f, 3.f);

    this->entities.reserve(Vulqian::Engine::ECS::MAX_ENTITIES + 1);

    this->load_vase();

    for (int i = 0; i != 400; i++) {
        float scale = randScale(generator);

        Vulqian::Engine::ECS::Entity entity = this->coordinator.create_entity();

        Vulqian::Engine::ECS::Components::Transform_TB_YXZ transform{};
        transform.scale = glm::vec3{scale, scale, scale};
        transform.rotation = glm::vec3{randRotation(generator), randRotation(generator), randRotation(generator)};
        transform.translation = glm::vec3{randPosition(generator), randPosition(generator), randPosition(generator)};

        Vulqian::Engine::ECS::Components::Mesh mesh{};
        mesh.model = Vulqian::Engine::Graphics::Model::create_model_from_file(this->device, Vulqian::Engine::Utils::colored_cube);

        this->coordinator.add_component(entity, Vulqian::Engine::ECS::Components::Transform_TB_YXZ{transform});
        this->coordinator.add_component(entity, Vulqian::Engine::ECS::Components::Mesh{mesh});
        this->entities.push_back(entity);
    }
}
