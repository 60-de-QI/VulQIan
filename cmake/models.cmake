    # Set the destination folder for the copied files
    set(destination_folder "${CMAKE_CURRENT_BINARY_DIR}/models")

    # Set the source folder containing the .spv files
    set(source_folder "${CMAKE_CURRENT_SOURCE_DIR}/models")

    # Copy .spv files to the destination folder
    file(COPY ${source_folder}/. DESTINATION ${destination_folder} FILES_MATCHING PATTERN "*.obj")

    message(STATUS "Copied models from " ${source_folder} " to " ${destination_folder})
