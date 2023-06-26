// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "../../Window/Window.hpp"
#include "../Device/Device.hpp"
#include "../SwapChain/SwapChain.hpp"

namespace Vulqian::Engine::Graphics {

class Renderer {
  public:
    Renderer(Vulqian::Engine::Window& window, Vulqian::Engine::Graphics::Device& device);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    VkRenderPass get_SwapChain_RenderPass(void) const noexcept { return this->swap_chain->getRenderPass(); }

    bool is_frame_in_progress(void) const noexcept { return this->is_frame_started; }
    VkCommandBuffer get_current_commanBuffer(void) const noexcept {
        assert(this->is_frame_started && "Cannot get command buffer when frame not in progress");
        return this->command_buffers[this->current_image_index];
    }

    VkCommandBuffer begin_frame(void);
    void end_frame(void);
    void begin_SwapChain_RenderPass(VkCommandBuffer command_buffer);
    void end_SwapChain_RenderPass(VkCommandBuffer command_buffer);

  private:
    void create_command_buffers(void);
    void free_command_buffers(void);
    void recreate_swap_chain(void);

    Vulqian::Engine::Window& window;
    Vulqian::Engine::Graphics::Device& device;
    std::unique_ptr<Vulqian::Engine::Graphics::SwapChain> swap_chain;
    std::vector<VkCommandBuffer> command_buffers;

    uint32_t current_image_index;
    bool is_frame_started{false};
};
} // namespace Vulqian::Engine::Graphics
