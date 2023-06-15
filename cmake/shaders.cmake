if (UNIX)
    add_custom_target(ShaderCompilation ALL
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/source/VulQIan/Shaders/compile.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running compile.sh for shaders"
    )
else()
    add_custom_target(ShaderCompilation ALL
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/source/VulQIan/Shaders/compile.bat
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running compile.bat for shaders"
    )
endif()

# Set the destination folder for the copied files
set(destination_folder "${CMAKE_CURRENT_BINARY_DIR}/Shaders")

# Set the source folder containing the .spv files
set(source_folder "${CMAKE_CURRENT_SOURCE_DIR}/source/VulQIan/Shaders")

# Copy .spv files to the destination folder
file(COPY ${source_folder}/. DESTINATION ${destination_folder} FILES_MATCHING PATTERN "*.spv")
