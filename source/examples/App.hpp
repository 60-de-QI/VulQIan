// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <VulQIan/Engine.hpp>
#include <VulQIan/Graphics/Pipeline/Pipeline.hpp>

class App {
   public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    void run(void) const;

   private:
    Vulqian::Engine::Window window{WIDTH, HEIGHT, "TEST"};
    Vulqian::Engine::Graphics::Pipeline pipeline{"/home/george/dev/VulQIan/source/VulQIan/Graphics/Shaders/simple_shader.vert.spv", "/home/george/dev/VulQIan/source/VulQIan/Graphics/Shaders/simple_shader.frag.spv"};
};
