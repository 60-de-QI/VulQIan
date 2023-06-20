// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <memory>
#include <string>

namespace Vulqian::Engine {

struct glfwDeleter {
    void operator()(GLFWwindow* ptr) const {
        glfwDestroyWindow(ptr);
    }
};

class Window {
  public:
    Window(unsigned short int w, unsigned short int h, std::string_view n);
    ~Window();

    // in order to respect RAII, we delete copy operations
    // That way we avoid copy for shared destructor
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    inline bool should_close() const noexcept { return glfwWindowShouldClose(this->window.get()); }

    VkExtent2D get_extent() const noexcept { return {static_cast<uint32_t>(this->width), static_cast<uint32_t>(this->height)}; }

    void create_window_surface(VkInstance instance, VkSurfaceKHR* surface) const;

  private:
    const unsigned short int width{640};
    const unsigned short int height{360};

    std::string name{"Window"};
    std::unique_ptr<GLFWwindow, glfwDeleter> window;
};
} // namespace Vulqian::Engine
