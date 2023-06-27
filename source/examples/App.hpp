// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <VulQIan/Engine.hpp>

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
    void generate_sierpinski_triangle(std::vector<Vulqian::Engine::Graphics::Model::Vertex>& vertices, int level, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);

  private:
    void load_world_objects(void);

    Vulqian::Engine::Window window{WIDTH, HEIGHT, "TEST"};
    Vulqian::Engine::Graphics::Device device{this->window};
    Vulqian::Engine::Graphics::Renderer renderer{this->window, this->device};

    std::vector<Vulqian::Engine::Graphics::WorldObject> world_objects;
};
