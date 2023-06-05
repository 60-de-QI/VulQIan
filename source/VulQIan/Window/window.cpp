// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "window.hpp"

namespace Vulqian::Engine {
    Window::Window(unsigned short int w, unsigned short int h, std::string_view n) : width(w), height(h), name(n) {
        this->init();
    }

    Window::~Window() {
        this->window.reset();
        glfwTerminate();
    }

    void    Window::destroy_ptr() {
        glfwDestroyWindow(this->window.get());
    }

    void Window::init() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        // this->window = std::make_shared<GLFWwindow>(glfwCreateWindow(this->width, this->height, this->name.c_str(), nullptr, nullptr), this->destroy_ptr{});
    }
}
