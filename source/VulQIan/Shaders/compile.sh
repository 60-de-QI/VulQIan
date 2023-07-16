#!/bin/bash

VULKAN_SDK_PATH="/home/george/vulkan/1.3.250.0/x86_64"
GLSLC_EXE="$VULKAN_SDK_PATH/bin/glslc"

# Check if glslc executable exists
if [ ! -x "$GLSLC_EXE" ]; then
  echo "Error: glslc executable not found in $VULKAN_SDK_PATH"
  exit 1
fi

echo "glslc found for shader compilation."

"$GLSLC_EXE" ./source/VulQIan/Shaders/simple_shader.vert -o ./source/VulQIan/Shaders/simple_shader.vert.spv
"$GLSLC_EXE" ./source/VulQIan/Shaders/simple_shader.frag -o ./source/VulQIan/Shaders/simple_shader.frag.spv

read -p "Press Enter to exit"
