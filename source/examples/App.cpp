// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "App.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <random>
#include <ranges>
#include <map>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

App::App() {
    this->global_pool = Vulqian::Engine::Graphics::Descriptors::DescriptorPool::Builder(this->device)
                            .setMaxSets(Vulqian::Engine::Graphics::SwapChain::MAX_FRAMES_IN_FLIGHT)
                            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Vulqian::Engine::Graphics::SwapChain::MAX_FRAMES_IN_FLIGHT)
                            .build();
    this->load_entities();
    this->load_systems();
}

void App::run() {
    std::vector<std::unique_ptr<Vulqian::Engine::Graphics::Buffer>> ubo_buffers(Vulqian::Engine::Graphics::SwapChain::MAX_FRAMES_IN_FLIGHT);

    for (int i{0}; i < ubo_buffers.size(); i++) {
        ubo_buffers[i] = std::make_unique<Vulqian::Engine::Graphics::Buffer>(
            this->device,
            sizeof(Vulqian::Engine::Graphics::Frames::GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            this->device.get_physical_device_properties().limits.minUniformBufferOffsetAlignment);
        ubo_buffers[i]->map();
    }

    auto globalSetLayout{Vulqian::Engine::Graphics::Descriptors::DescriptorSetLayout::Builder(this->device)
                             .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                             .build()};

    std::vector<VkDescriptorSet> globalDescriptorSets(Vulqian::Engine::Graphics::SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++) {
        auto bufferInfo{ubo_buffers[i]->descriptorInfo()};
        Vulqian::Engine::Graphics::Descriptors::DescriptorWriter(*globalSetLayout, *global_pool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
    }

    Vulqian::Engine::Graphics::RenderSystem    render_system{this->device, this->renderer.get_SwapChain_RenderPass(), globalSetLayout->getDescriptorSetLayout()};
    Vulqian::Engine::ECS::Systems::PointLights point_light_system{this->device, this->renderer.get_SwapChain_RenderPass(), globalSetLayout->getDescriptorSetLayout()};
    Vulqian::Engine::Graphics::Camera          camera{};

    camera.set_view_target(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    auto viewer_entity{this->coordinator.create_entity()};

    Vulqian::Engine::ECS::Components::Transform_TB_YXZ transform{};
    transform.translation.z = -2.5f;
    this->coordinator.add_component(viewer_entity, Vulqian::Engine::ECS::Components::Transform_TB_YXZ{transform});

    Vulqian::Engine::Input::KeyboardMovementController camera_controller{};
    Vulqian::Engine::Input::MouseCameraController      mouse_controller{};

    auto current_time{std::chrono::high_resolution_clock::now()};

    while (!this->window.should_close()) {
        glfwPollEvents();

        auto  new_time{std::chrono::high_resolution_clock::now()};
        float frame_time{std::chrono::duration<float, std::chrono::seconds::period>(new_time - current_time).count()};
        current_time = new_time;

        mouse_controller.update_camera_orientation(this->window.get_window(), frame_time, transform);
        camera_controller.move_in_plane_xz(this->window.get_window(), frame_time, transform);
        camera.set_view_YXZ(transform.translation, transform.rotation);

        float aspect = this->renderer.get_aspect_ratio();
        camera.set_perspective_projection(glm::radians(50.f), aspect, .1f, 1000.f);

        if (auto command_buffer = this->renderer.begin_frame()) {
            int                                     frame_index{this->renderer.get_frame_index()};
            Vulqian::Engine::Graphics::Frames::Info frame_info{
                0,
                frame_time,
                nullptr,
                camera,
                globalDescriptorSets[frame_index]};

            frame_info.command_buffer = command_buffer;

            // update objects and memory
            Vulqian::Engine::Graphics::Frames::GlobalUbo ubo{};
            ubo.projection = camera.get_projection();
            ubo.view = camera.get_view();
            ubo.inverseView = camera.get_inverse_view();
            point_light_system.update(frame_info, ubo, this->coordinator, this->entities);
            ubo_buffers[frame_index]->writeToBuffer(&ubo);
            ubo_buffers[frame_index]->flush();

            // rendering phase /!\ the order matters
            this->renderer.begin_SwapChain_RenderPass(command_buffer);

            // 1. Render opaque objects first
            render_system.render_opaque_entities_only(frame_info, this->entities, this->coordinator);

            // 2. Collect and sort all transparent objects (meshes + lights)
            std::map<float, std::pair<std::string, Vulqian::Engine::ECS::Entity>> transparent_objects;
            glm::vec3                                                             camera_pos = camera.get_position();

            // Add transparent meshes
            for (const auto& entity : this->entities) {
                if (coordinator.has_component<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>(entity) &&
                    coordinator.has_component<Vulqian::Engine::ECS::Components::Mesh>(entity) &&
                    coordinator.has_component<Vulqian::Engine::ECS::Components::Transparency>(entity)) {
                    auto const& transform = coordinator.get_component<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>(entity);
                    auto        offset = camera_pos - transform.translation;
                    float       depth = glm::dot(offset, offset);
                    transparent_objects[depth] = {"mesh", entity};
                }
            }

            // Add lights
            for (const auto& entity : this->entities) {
                if (coordinator.has_component<Vulqian::Engine::ECS::Components::PointLight>(entity)) {
                    auto const& transform = coordinator.get_component<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>(entity);
                    auto        offset = camera_pos - transform.translation;
                    float       depth = glm::dot(offset, offset);
                    transparent_objects[depth] = {"light", entity};
                }
            }

            // 3. Render transparent objects back-to-front
            for (auto it = transparent_objects.rbegin(); it != transparent_objects.rend(); ++it) {
                if (it->second.first == "mesh") {
                    render_system.render_transparent_entity(frame_info, it->second.second, this->coordinator);
                } else {
                    point_light_system.render_single_light(frame_info, it->second.second, this->coordinator);
                }
            }

            this->renderer.end_SwapChain_RenderPass(command_buffer);
            this->renderer.end_frame();
        }
    }
    vkDeviceWaitIdle(this->device.get_device());
}

void App::load_vase(void) {
    Vulqian::Engine::ECS::Entity                       smooth_vase{this->coordinator.create_entity()};
    Vulqian::Engine::ECS::Components::Transform_TB_YXZ transform{};

    transform.scale = glm::vec3{3.f, 1.5f, 3.f};
    transform.translation = glm::vec3{-.5f, .5f, .0f};

    Vulqian::Engine::ECS::Components::Mesh mesh{};
    mesh.model = Vulqian::Engine::Graphics::Model::create_model_from_file(this->device, Vulqian::Engine::Utils::smooth_vase);

    this->coordinator.add_component(smooth_vase, Vulqian::Engine::ECS::Components::Transform_TB_YXZ{transform});
    this->coordinator.add_component(smooth_vase, Vulqian::Engine::ECS::Components::Mesh{mesh});
    this->entities.push_back(smooth_vase);

    // flat vase
    Vulqian::Engine::ECS::Entity                       flat_vase = this->coordinator.create_entity();
    Vulqian::Engine::ECS::Components::Transform_TB_YXZ transform_flat{};

    transform_flat.scale = {3.f, 1.5f, 3.f};
    transform_flat.translation = {.5f, .5f, .0f};

    Vulqian::Engine::ECS::Components::Mesh flat_mesh{};
    flat_mesh.model = Vulqian::Engine::Graphics::Model::create_model_from_file(this->device, Vulqian::Engine::Utils::flat_vase);

    this->coordinator.add_component(flat_vase, Vulqian::Engine::ECS::Components::Transform_TB_YXZ{transform_flat});
    this->coordinator.add_component(flat_vase, Vulqian::Engine::ECS::Components::Mesh{flat_mesh});
    this->entities.push_back(flat_vase);

    // flat plane for lights
    Vulqian::Engine::ECS::Entity                       quad{this->coordinator.create_entity()};
    Vulqian::Engine::ECS::Components::Transform_TB_YXZ transform_quad{};

    transform_quad.scale = {3.f, 1.f, 3.f};
    transform_quad.translation = {0.f, .5f, 0.f};

    Vulqian::Engine::ECS::Components::Mesh quad_mesh{};
    quad_mesh.model = Vulqian::Engine::Graphics::Model::create_model_from_file(this->device, Vulqian::Engine::Utils::quad);

    this->coordinator.add_component(quad, Vulqian::Engine::ECS::Components::Transform_TB_YXZ{transform_quad});
    this->coordinator.add_component(quad, Vulqian::Engine::ECS::Components::Mesh{quad_mesh});
    this->entities.emplace_back(quad);
}

void App::load_systems(void) {
    this->physics_system = this->coordinator.register_system<Vulqian::Engine::ECS::Systems::Physics>();

    Vulqian::Engine::ECS::Signature physics_signature;
    physics_signature.set(this->coordinator.get_component_type<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>());
    physics_signature.set(this->coordinator.get_component_type<Vulqian::Engine::ECS::Components::Mesh>());
    physics_signature.set(this->coordinator.get_component_type<Vulqian::Engine::ECS::Components::PointLight>());

    this->coordinator.set_system_signature<Vulqian::Engine::ECS::Systems::Physics>(physics_signature);
}

void App::load_transparent_quad(void) {
    // Create transparent quad positioned in front of the vases
    Vulqian::Engine::ECS::Entity                       transparent_quad{this->coordinator.create_entity()};
    Vulqian::Engine::ECS::Components::Transform_TB_YXZ transform{};

    // Position and orient the quad properly
    transform.scale = glm::vec3{3.0f, 3.0f, 1.0f};
    transform.translation = glm::vec3{0.0f, 0.5f, -0.8f};

    // Rotate the quad to face the camera - assuming your quad is in XY plane by default
    // transform.rotation = glm::vec3{0.0f, 0.0f, 0.0f};      // Try no rotation first
    transform.rotation = glm::vec3{glm::radians(90.0f), 0.0f, 0.0f};

    Vulqian::Engine::ECS::Components::Mesh mesh{};
    mesh.model = Vulqian::Engine::Graphics::Model::create_model_from_file(this->device, Vulqian::Engine::Utils::quad);

    Vulqian::Engine::ECS::Components::Transparency transparency{};
    transparency.alpha = 0.4f;                         // 40% opacity for nice transparency effect
    transparency.color = glm::vec3{0.2f, 0.6f, 1.0f};  // Light blue tint
    transparency.requiresDepthSorting = true;

    this->coordinator.add_component(transparent_quad, transform);
    this->coordinator.add_component(transparent_quad, mesh);
    this->coordinator.add_component(transparent_quad, transparency);

    this->entities.push_back(transparent_quad);
}

void App::load_entities(void) {
    // ECS
    this->coordinator.init();
    this->coordinator.register_component<Vulqian::Engine::ECS::Components::Transform_TB_YXZ>();
    this->coordinator.register_component<Vulqian::Engine::ECS::Components::Mesh>();
    this->coordinator.register_component<Vulqian::Engine::ECS::Components::PointLight>();
    this->coordinator.register_component<Vulqian::Engine::ECS::Components::Transparency>();  // Add this line

    std::default_random_engine            generator;
    std::uniform_real_distribution<float> randPosition(-100.0f, 100.0f);
    std::uniform_real_distribution<float> randRotation(.0f, 3.0f);
    std::uniform_real_distribution<float> randScale(0.5f, 3.f);

    this->entities.reserve(Vulqian::Engine::ECS::MAX_ENTITIES + 1);

    this->load_vase();

    // Create transparent quad in front of the vases
    this->load_transparent_quad();

    // Create regular mesh entities (with Transform + Mesh)
    for (int i = 0; i != 400; i++) {
        float scale{randScale(generator)};

        Vulqian::Engine::ECS::Entity entity{this->coordinator.create_entity()};

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

    // Create light entities (with Transform + PointLight, but NO Mesh)
    std::vector<glm::vec3> lightColors{
        {1.f, .1f, .1f},  // Red
        {.1f, .1f, 1.f},  // Blue
        {.1f, 1.f, .1f},  // Green
        {1.f, 1.f, .1f},  // Yellow
        {.1f, 1.f, 1.f},  // Cyan
        {1.f, 1.f, 1.f}   // White
    };

    for (size_t i = 0; i < lightColors.size(); ++i) {
        const auto& color{lightColors[i]};

        Vulqian::Engine::ECS::Entity                       light{this->coordinator.create_entity()};
        Vulqian::Engine::ECS::Components::Transform_TB_YXZ transform{};

        transform.scale = glm::vec3{0.1f, 0.1f, 0.1f};
        transform.rotation = glm::vec3{0.0f, 0.0f, 0.0f};

        auto rotateLight{glm::rotate(
            glm::mat4(1.f),
            (i * glm::two_pi<float>()) / lightColors.size(),
            {0.f, -1.f, 0.f})};
        transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));

        Vulqian::Engine::ECS::Components::PointLight pointLight{};
        pointLight.lightIntensity = 0.2f;
        pointLight.color = color;

        this->coordinator.add_component(light, transform);
        this->coordinator.add_component(light, pointLight);
        this->entities.push_back(light);
    }
}
