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

    // ECS
    this->coordinator.init();
    this->coordinator.register_component<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>();
    // auto physics_system = this->coordinator.register_system<Vulqian::Engine::ECS::Systems::Physics>();

    // this->signature.set(this->coordinator.get_component_type<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>());
    // this->coordinator.set_system_signature<Vulqian::Engine::ECS::Systems::Physics>(signature);
}

void App::run() {
    Vulqian::Engine::Graphics::RenderSystem render_system{this->device, this->renderer.get_SwapChain_RenderPass()};
    Vulqian::Engine::Graphics::Camera       camera{};

    camera.set_view_target(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    // Vulqian::Engine::ECS::Entity viewer_object = Vulqian::Engine::ECS::WorldObject::create_game_object();
    // Vulqian::Engine::ECS::Entity viewer_object = Vulqian::Engine::ECS::Entity::create();
    // auto&                        transform_component = world_objects_registry.add<Vulqian::Engine::ECS::TransformComponent>(viewer_object);

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

void App::load_world_objects(void) {
    std::shared_ptr<Vulqian::Engine::Graphics::Model> model = Vulqian::Engine::Graphics::Model::create_model_from_file(this->device, "./conan-build/Debug/models/smooth_vase.obj");

    auto object = Vulqian::Engine::Graphics::WorldObject::create_game_object();
    object.model = model;
    object.transform.translation = {.0f, .0f, 2.5f};
    object.transform.scale = glm::vec3{3.f};

    this->world_objects.push_back(std::move(object));
}

// void App::load_world_objects(void) {
//     std::shared_ptr<Vulqian::Engine::Graphics::Model> lveModel =
//         Vulqian::Engine::Graphics::Model::create_model_from_file(this->device, "./conan-build/Debug/models/smooth_vase.obj");

//     Vulqian::Engine::ECS::Entity object = Vulqian::Engine::ECS::Entity::create();

//     auto& transform_component = world_objects_registry.add<Vulqian::Engine::ECS::TransformComponent>(object);
//     transform_component.translation = {.0f, .0f, 2.5f};
//     transform_component.scale = glm::vec3{3.f};
// }
