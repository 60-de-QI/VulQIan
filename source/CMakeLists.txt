# Add the main executable
add_subdirectory(engine)

if(VULQIAN_BUILD_EXAMPLE)
    add_subdirectory(examples)
endif()
