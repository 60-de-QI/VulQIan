// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include "VulQian/Engine.hpp"

class App {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        void run(void);

    private:
        Vulqian::Engine::Window window{WIDTH, HEIGHT, "TEST"};
};
