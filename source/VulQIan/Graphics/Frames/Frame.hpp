// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "../Camera/Camera.hpp"

#include <vulkan/vulkan.h>

namespace Vulqian::Engine::Graphics::Frames {

struct Info {
    int                                frame_index;
    float                              frame_time;
    VkCommandBuffer                    command_buffer;
    Vulqian::Engine::Graphics::Camera& camera;
};

} // namespace Vulqian::Engine::Graphics::Frames
