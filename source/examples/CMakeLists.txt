cmake_minimum_required(VERSION 3.26)
project(vulqian_example)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED)

# Add the example executable
add_executable(vulqian_example App.cpp main.cpp)

# Add additional libraries or dependencies as needed
target_include_directories(vulqian_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(vulqian_example PRIVATE VulQIan)
