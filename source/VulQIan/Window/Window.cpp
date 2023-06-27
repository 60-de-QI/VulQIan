// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#include <bit>

#include "../Exception/Exception.hpp"
#include "Window.hpp"

namespace Vulqian::Engine {
Window::Window(int w, int h, std::string_view n) : width(w), height(h), name(n) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    this->window.reset(glfwCreateWindow(this->width, this->height, this->name.c_str(), nullptr, nullptr));
    glfwSetWindowUserPointer(this->window.get(), this);
    glfwSetFramebufferSizeCallback(this->window.get(), frame_buffer_resized_callback);
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

void Window::frame_buffer_resized_callback(GLFWwindow* window, int width, int height) {
    auto new_window = std::bit_cast<Vulqian::Engine::Window*>(glfwGetWindowUserPointer(window));

    new_window->frame_buffer_resized = true;
    new_window->width = width;
    new_window->height = height;
}

} // namespace Vulqian::Engine
