// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <string>
#include <memory>

namespace Vulqian::Engine {
    class Window {
        public:
            Window(unsigned short int w, unsigned short int h, std::string_view n);
            ~Window();

            void destroy_ptr();

        private:

            void init();

            const unsigned short int width{640};
            const unsigned short int height{360};

            std::string                 name{"My Window"};
            std::shared_ptr<GLFWwindow> window;
    };
}
