@echo off

set VULKAN_SDK_PATH=D:\VulkanSDK\1.3.250.0
set GLSLC_EXE=%VULKAN_SDK_PATH%\Bin\glslc.exe

REM Check if glslc.exe exists
if not exist "%GLSLC_EXE%" (
  echo Error: glslc.exe not found in %VULKAN_SDK_PATH%
  exit /b 1
)

echo glslc.exe found for shader compilation.

"%GLSLC_EXE%" ./source/VulQIan/Shaders/simple_shader.vert -o ./source/VulQIan/Shaders/simple_shader.vert.spv
"%GLSLC_EXE%" ./source/VulQIan/Shaders/simple_shader.frag -o ./source/VulQIan/Shaders/simple_shader.frag.spv

pause
