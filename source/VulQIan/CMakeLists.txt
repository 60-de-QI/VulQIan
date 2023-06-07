# Add the engine library
file(GLOB_RECURSE VULQIAN_SOURCES LIST_DIRECTORIES false
    "*.cpp"
    "*.hpp"
)

add_library(VulQIan ${VULQIAN_SOURCES})

# Include directories for the engine library
target_include_directories(VulQIan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${VULKAN_SDK_PATH}/include)
target_link_libraries(VulQIan ${Vulkan_LIBRARIES} glfw glm::glm)
