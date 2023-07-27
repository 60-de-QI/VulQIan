    # Add the test executable and set its sources
    file(GLOB_RECURSE VULQIAN_TEST_SOURCES
        "*.cpp"
    )
    add_executable(VulQIanTests ${VULQIAN_TEST_SOURCES})

    # Set the binary output directory for tests to the same as the main executable
    set_target_properties(VulQIanTests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )

    # Link the engine library and any other dependencies needed for tests
    target_link_libraries(VulQIanTests PRIVATE VulQIan)

    # Register the test with CTest so it can be executed by 'ctest' command
    add_test(NAME VulQIanTests COMMAND VulQIanTests)
