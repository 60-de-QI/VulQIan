// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <string>
#include <vector>

namespace Vulqian::Engine::Graphics {
class Pipeline {
   public:
    Pipeline(const std::string& vert_filepath, const std::string& frag_filepath);

   private:
    static std::vector<char> read_file(const std::string& path);

    void create_graphics_pipeline(const std::string& vert_filepath, const std::string& frag_filepath);
};
}  // namespace Vulqian::Engine::Graphics
