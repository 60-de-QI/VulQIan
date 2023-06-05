# Add the main executable
add_subdirectory(VulQIan)

if(VULQIAN_BUILD_EXAMPLE)
    add_subdirectory(examples)
endif()
