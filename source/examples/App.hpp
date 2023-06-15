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
    ~App();

    App(const App &) = delete;
    App &operator=(const App&) = delete;

   private:
    void create_pipeline_layout();
    void create_pipeline();
    void create_command_buffers();
    void draw_frame();

    Vulqian::Engine::Window window{WIDTH, HEIGHT, "TEST"};
    Vulqian::Engine::Graphics::Device device{this->window};
    Vulqian::Engine::Graphics::SwapChain swap_chain{device, window.get_extent()};

    VkPipelineLayout pipeline_layout;

    std::unique_ptr<Vulqian::Engine::Graphics::Pipeline> pipeline;
    std::vector<VkCommandBuffer> command_buffers;
};
