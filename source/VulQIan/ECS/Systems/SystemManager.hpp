// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include "System.hpp"
#include "../Types.hpp"

#include <cassert>
#include <memory>
#include <unordered_map>

namespace Vulqian::Engine::ECS {

class SystemManager {
  public:
    template <typename T>
    std::shared_ptr<T> register_system();

    template <typename T>
    void set_signature(Signature signature);

    void entity_destroyed(Entity entity);
    void entity_signature_changed(Entity entity, Signature entitySignature);

  private:
    // Map from system type string pointer to a signature
    std::unordered_map<const char*, Signature> signatures{};

    // Map from system type string pointer to a system pointer
    std::unordered_map<const char*, std::shared_ptr<System>> systems{};
};

} // namespace Vulqian::Engine::ECS
