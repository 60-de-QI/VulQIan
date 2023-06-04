# Find Vulkan package
set(VULKAN_SDK_PATH "$ENV{VULKAN_SDK}")

if(DEFINED ENV{VULKAN_SDK})
    find_package(Vulkan REQUIRED)
else()
    message(STATUS "VULKAN_SDK: $ENV{VULKAN_SDK}")
    message(FATAL_ERROR " VULKAN SDK NOT FOUND")
endif()

find_package(glfw3 3.3.8 REQUIRED)
if(NOT glfw3_FOUND)
    message(FATAL_ERROR "GLFW NOT FOUND")
endif()

find_package(glm REQUIRED)
if(NOT glm_FOUND)
    message(FATAL_ERROR "GLM NOT FOUND")
endif()
