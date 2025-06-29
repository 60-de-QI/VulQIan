// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <limits>
#include <string>

namespace Vulqian::Engine::Utils {

// from: https://stackoverflow.com/a/57595105
template <typename T, typename... Rest>
void hash_combine(std::size_t& seed, const T& v, const Rest&... rest) {
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hash_combine(seed, rest), ...);
};

const std::string colored_cube{"./conan-build/models/colored_cube.obj"};
const std::string smooth_vase{"./conan-build/models/smooth_vase.obj"};
const std::string flat_vase{"./conan-build/models/flat_vase.obj"};
const std::string quad{"./conan-build/models/quad.obj"};

} // namespace Vulqian::Engine::Utils
