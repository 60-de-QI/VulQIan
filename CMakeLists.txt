cmake_minimum_required(VERSION 3.22.1)
project(VulQIan)

# Include all CMake files in the cmake directory
file(GLOB CMAKE_FILES cmake/*.cmake)

foreach(CMAKE_FILE ${CMAKE_FILES})
    include(${CMAKE_FILE})
endforeach()

if (VULQIAN_BUILD_TESTS)
    enable_testing()
endif()

# Add the source library
add_subdirectory(source)
