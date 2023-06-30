// Vulquian - Custom Vulkan Engine
// Copyright (C) 60-de-QI - All rights reserved
// This software is provided 'as is' and without any warranty, express or implied.
// The author(s) disclaim all liability for damages resulting from the use or misuse of this software.

#pragma once

#include <source_location>
#include <stdexcept>
#include <unordered_map>

namespace Vulqian::Exception {

template <typename Derived>
class general_exception : public std::exception {
  private:
    std::string          error;
    std::source_location location;

  public:
    explicit general_exception(std::string const& what, std::source_location loc = std::source_location::current())
        : error{get_prefix() + what + "\nFile: " + loc.file_name() + "\nLine: " + std::to_string(loc.line()) + "\nFunction: " + loc.function_name()}, location(loc) {}

    const char* what() const noexcept override {
        return this->error.c_str();
    }

    // Get the prefix for the derived class
    static std::string get_prefix() {
        return Derived::get_prefix();
    }
};

class unavailable : public Vulqian::Exception::general_exception<unavailable> {
  public:
    using general_exception::general_exception;
    static std::string get_prefix() { return "requested resource is unavailable : "; }
};

class failed_to_find : public Vulqian::Exception::general_exception<failed_to_find> {
  public:
    using general_exception::general_exception;
    static std::string get_prefix() { return "failed to find : "; }
};

class failed_to_create : public Vulqian::Exception::general_exception<failed_to_create> {
  public:
    using general_exception::general_exception;
    static std::string get_prefix() { return "failed to create : "; }
};

class failed_to_open : public Vulqian::Exception::general_exception<failed_to_open> {
  public:
    using general_exception::general_exception;
    static std::string get_prefix() { return "failed to open :"; }
};

class failed_to_setup : public Vulqian::Exception::general_exception<failed_to_setup> {
  public:
    using general_exception::general_exception;
    static std::string get_prefix() { return "failed to setup : "; }
};

class failed_to_allocate : public Vulqian::Exception::general_exception<failed_to_allocate> {
  public:
    using general_exception::general_exception;
    static std::string get_prefix() { return "failed to allocate : "; }
};

class failed_to_bind : public Vulqian::Exception::general_exception<failed_to_bind> {
  public:
    using general_exception::general_exception;
    static std::string get_prefix() { return "failed to bind : "; }
};

class failed_to_load : public Vulqian::Exception::general_exception<failed_to_load> {
  public:
    using general_exception::general_exception;
    static std::string get_prefix() { return "failed to load : "; }
};

class missing_requirements : public Vulqian::Exception::general_exception<missing_requirements> {
  public:
    using general_exception::general_exception;
    static std::string get_prefix() { return "missing requirements : "; }
};

} // namespace Vulqian::Exception
