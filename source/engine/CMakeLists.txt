# Add the engine library
file(GLOB_RECURSE VULQIAN_SOURCES LIST_DIRECTORIES false
    "*.cpp"
)

add_library(vulqian ${VULQIAN_SOURCES})

# Include directories for the engine library
target_include_directories(vulqian PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${VULKAN_SDK_PATH}/Include)
target_link_libraries(vulqian ${Vulkan_LIBRARIES} glfw glm::glm)
