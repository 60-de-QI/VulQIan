// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <VulQIan/Engine.hpp>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

class App {
  public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    void run(void);

    App();
    ~App() = default;

    App(const App&) = delete;
    App& operator=(const App&) = delete;

  private:
    void load_entities(void);
    void load_systems(void);
    void load_vase(void);

    Vulqian::Engine::Window             window{WIDTH, HEIGHT, "VulQIan - Demo"};
    Vulqian::Engine::Graphics::Device   device{this->window};
    Vulqian::Engine::Graphics::Renderer renderer{this->window, this->device};

    // Descriptor Sets ! order of declaration matters
    std::unique_ptr<Vulqian::Engine::Graphics::Descriptors::DescriptorPool> global_pool;

    // ECS
    Vulqian::Engine::ECS::Coordinator coordinator{};
    Vulqian::Engine::ECS::Signature   signature{};

    std::shared_ptr<Vulqian::Engine::ECS::Systems::Physics> physics_system;
    std::vector<Vulqian::Engine::ECS::Entity>               entities;
};
