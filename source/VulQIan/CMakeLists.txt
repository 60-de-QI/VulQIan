# Add the engine library
file(GLOB_RECURSE VULQIAN_SOURCES LIST_DIRECTORIES false
    "*.cpp"
    "*.hpp"
)

add_library(VulQIan ${VULQIAN_SOURCES})

# Include directories for the engine library
target_include_directories(VulQIan PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${VULKAN_SDK_PATH}/include
)

if (VULQIAN_BUILD_TESTS)
    # Locate and link Google Test
    find_package(GTest REQUIRED)
    target_link_libraries(VulQIan 
        ${Vulkan_LIBRARIES} 
        glfw 
        glm::glm 
        tinyobjloader::tinyobjloader 
        stb::stb
        GTest::GTest 
        GTest::Main
    )
    enable_testing()
    add_subdirectory(tests)
else()
    target_link_libraries(VulQIan 
        ${Vulkan_LIBRARIES} 
        glfw 
        glm::glm 
        tinyobjloader::tinyobjloader 
        stb::stb  # Link STB
    )
endif()
