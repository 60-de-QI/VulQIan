#!/bin/bash

GLSLC_EXE="$VULKAN_SDK/bin/glslc"

# Check if glslc executable exists
if [ ! -x "$GLSLC_EXE" ]; then
  echo "Error: glslc executable not found in $VULKAN_SDK_PATH"
  exit 1
fi

echo "executing shader compilation script from: $PWD"
echo "glslc found for shader compilation."

"$GLSLC_EXE" ./source/VulQIan/Shaders/simple_shader.vert -o ./source/VulQIan/Shaders/simple_shader.vert.spv
"$GLSLC_EXE" ./source/VulQIan/Shaders/simple_shader.frag -o ./source/VulQIan/Shaders/simple_shader.frag.spv

"$GLSLC_EXE" ./source/VulQIan/Shaders/point_light.vert -o ./source/VulQIan/Shaders/point_light.vert.spv
"$GLSLC_EXE" ./source/VulQIan/Shaders/point_light.frag -o ./source/VulQIan/Shaders/point_light.frag.spv
