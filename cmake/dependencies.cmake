# Find Vulkan package
set(VULKAN_SDK_PATH "$ENV{VK_SDK_PATH}")

message(STATUS "VK_SDK_PATH: $ENV{VK_SDK_PATH}")

if(DEFINED ENV{VK_SDK_PATH})
    find_package(Vulkan REQUIRED)
else()
    message(VULKAN_SDK_PATH: < $ENV{VK_SDK_PATH} >)
    message(FATAL_ERROR " VULKAN SDK NOT FOUND")
endif()
