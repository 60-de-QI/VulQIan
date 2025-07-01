// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <vulkan/vulkan.h>

#include <array>

#include "../Camera/Camera.hpp"

namespace Vulqian::Engine::Graphics::Frames {

static constexpr unsigned short int MAX_LIGHTS{10};

struct Info {
    int                                frame_index;
    float                              frame_time;
    VkCommandBuffer                    command_buffer;
    Vulqian::Engine::Graphics::Camera& camera;
    VkDescriptorSet                    global_descriptor_set;
};

struct PointLight {
    glm::vec4 position{};  // ignore w
    glm::vec4 color{};     // w is intensity
};

struct GlobalUbo {
    glm::mat4                          projection{1.f};
    glm::mat4                          view{1.f};
    glm::mat4                          inverseView{1.f};
    glm::vec4                          ambientLightColor{1.f, 1.f, 1.f, .02f};  // w is intensity
    std::array<PointLight, MAX_LIGHTS> pointLights;
    int                                numLights;
};

}  // namespace Vulqian::Engine::Graphics::Frames
