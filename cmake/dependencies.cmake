# Find Vulkan package
set(VULKAN_SDK_PATH "$ENV{VULKAN_SDK}")

if(DEFINED ENV{VK_SDK_PATH})
    find_package(Vulkan REQUIRED)
else()
    message(STATUS "VK_SDK_PATH: $ENV{VK_SDK_PATH}")
    message(FATAL_ERROR " VULKAN SDK NOT FOUND")
endif()
