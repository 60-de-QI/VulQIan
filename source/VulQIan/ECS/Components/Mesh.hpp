// Vulqian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "../../Graphics/Model/Model.hpp"
#include "../../Graphics/Textures/Texture.hpp"

namespace Vulqian::Engine::ECS::Components {

struct Mesh {
    std::shared_ptr<Vulqian::Engine::Graphics::Model>   model{};
    std::shared_ptr<Vulqian::Engine::Graphics::Textures::Texture> diffuseTexture{};

    // Factory methods for easy creation
    static Mesh create(std::shared_ptr<Vulqian::Engine::Graphics::Model> model) {
        Mesh mesh;
        mesh.model = model;
        // diffuseTexture will be nullptr - renderer will use default
        return mesh;
    }

    static Mesh createWithTexture(std::shared_ptr<Vulqian::Engine::Graphics::Model>   model,
                                  std::shared_ptr<Vulqian::Engine::Graphics::Textures::Texture> texture) {
        Mesh mesh;
        mesh.model = model;
        mesh.diffuseTexture = texture;
        return mesh;
    }

    // Convenience method
    bool hasTexture() const noexcept { return diffuseTexture != nullptr; }
};

}  // namespace Vulqian::Engine::ECS::Components
