# Add the engine library
file(GLOB_RECURSE VULQIAN_SOURCES LIST_DIRECTORIES false
    "*.cpp"
    "*.hpp"
)

add_library(VulQIan ${VULQIAN_SOURCES})

# Include directories for the engine library
target_include_directories(VulQIan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${VULKAN_SDK_PATH}/include)

if (VULQIAN_BUILD_TESTS)
    # Locate and link Google Test
    find_package(GTest REQUIRED)
    target_link_libraries(VulQIan ${Vulkan_LIBRARIES} glfw glm::glm tinyobjloader::tinyobjloader GTest::GTest GTest::Main)

    # Add the test executable and set its sources
    file(GLOB_RECURSE VULQIAN_TEST_SOURCES LIST_DIRECTORIES false
        "tests/*.cpp"
    )
    add_executable(VulQIanTests ${VULQIAN_TEST_SOURCES})

    # Link the engine library and any other dependencies needed for tests
    target_link_libraries(VulQIanTests PRIVATE VulQIan)

    # Register the test with CTest so it can be executed by 'ctest' command
    enable_testing()
    add_test(NAME VulQIanTests COMMAND VulQIanTests)
else()
    target_link_libraries(VulQIan ${Vulkan_LIBRARIES} glfw glm::glm tinyobjloader::tinyobjloader)
endif()
