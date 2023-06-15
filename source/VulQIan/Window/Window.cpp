// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include "Window.hpp"
#include "../Exception/Exception.hpp"

namespace Vulqian::Engine {
Window::Window(unsigned short int w, unsigned short int h, std::string_view n) : width(w), height(h), name(n) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    this->window.reset(glfwCreateWindow(this->width, this->height, this->name.c_str(), nullptr, nullptr));
}

Window::~Window() {
    this->window.release();
    glfwTerminate();
}

void Window::create_window_surface(VkInstance instance, VkSurfaceKHR* surface) const {
    if (glfwCreateWindowSurface(instance, this->window.get(), nullptr, surface) != VK_SUCCESS) {
        throw Vulqian::Exception::failed_to_create("window surface");
    }
}

}  // namespace Vulqian::Engine
