// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <memory>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace Vulqian::Engine {

struct glfwDeleter {
    void operator()(GLFWwindow* ptr) const {
        glfwDestroyWindow(ptr);
    }
};

class Window {
  public:
    Window(int w, int h, std::string_view n);
    ~Window();

    // in order to respect RAII, we delete copy operations
    // That way we avoid copy for shared destructor
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    inline bool should_close() const noexcept { return glfwWindowShouldClose(this->window.get()); }

    VkExtent2D get_extent() const noexcept { return {static_cast<uint32_t>(this->width), static_cast<uint32_t>(this->height)}; }

    void create_window_surface(VkInstance instance, VkSurfaceKHR* surface) const;
    bool was_window_resized(void) const noexcept { return this->frame_buffer_resized; }
    void reset_window_resized_flage(void) { this->frame_buffer_resized = false; }

  private:
    static void frame_buffer_resized_callback(GLFWwindow* window, int width, int height);
    int         width{640};
    int         height{360};
    bool        frame_buffer_resized{false};

    std::string                              name{"Window"};
    std::unique_ptr<GLFWwindow, glfwDeleter> window;
};
} // namespace Vulqian::Engine
