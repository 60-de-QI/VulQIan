cmake_minimum_required(VERSION 3.12)
project(vulqian_example)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add the example executable
add_executable(vulqian_example main.cpp)

# Link against the Vulquian library
target_link_libraries(vulqian_example vulqian)

# Add additional libraries or dependencies as needed
