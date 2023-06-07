// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma on

#include <stdexcept>

namespace Vulqian::Exception {
class general_exception : public std::exception {
   private:
    std::string error;

   protected:
    explicit general_exception(std::string const& e) : error{"A general exception has occured: " + e} {}
};

class unavailable : public general_exception {
   public:
    explicit unavailable(std::string const& what) : general_exception("Resource requested, but unavailable : " + what) {}
};

class failed_to_find : public general_exception {
    public:
        explicit failed_to_find(std::string const& what) : general_exception("failed to find : " + what) {}
};

class failed_to_create : public general_exception {
    public:
        explicit failed_to_create(std::string const& what) : general_exception("failed to create : " + what) {}
};

class failed_to_open : public general_exception {
    public:
        explicit failed_to_open(std::string const& what) : general_exception("failed to open : " + what) {}
};

class failed_to_setup : public general_exception {
    public:
        explicit failed_to_setup(std::string const& what) : general_exception("failed to setup : " + what) {}
};

class failed_to_allocate : public general_exception {
    public:
        explicit failed_to_allocate(std::string const& what) : general_exception("failed to allocate : " + what) {}
};

class failed_to_bind : public general_exception {
    public:
        explicit failed_to_bind(std::string const& what) : general_exception("failed to bind : " + what) {}
};

class missing_requirements : public general_exception {
    public:
        explicit missing_requirements(std::string const& what) : general_exception("missing requirements : " + what) {}
};

}  // namespace Vulqian::Exception
