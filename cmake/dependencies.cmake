# Find Vulkan package
set(VULKAN_SDK_PATH "$ENV{VULKAN_SDK}")

if(DEFINED ENV{VULKAN_SDK})
    find_package(Vulkan REQUIRED)
else()
    message(STATUS "VULKAN_SDK: $ENV{VULKAN_SDK}")
    message(FATAL_ERROR " VULKAN SDK NOT FOUND")
endif()
