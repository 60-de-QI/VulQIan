# VulQIan: The Epic Intelligence Engine

This is a simple 3D engine built using Vulkan, C++20, CMake and Visual Studio Code. The engine aims to provide a foundation for creating real-time 3D applications. This is mostly a way for me to explore concepts and techniques used in graphics programming on a lower level of implementation.

## Features

- Window creation and Vulkan initialization.
- Vulkan swap chain for image presentation.
- Vertex and index buffers for geometry storage.
- Shaders and pipeline setup for rendering.
- Rendering of basic primitives (e.g., triangles, Cubes).
- Uniform buffers for passing data to shaders.

## Roadmap

1.  :heavy_check_mark: Window Creation and Vulkan Initialization
2.  :heavy_check_mark: Vulkan Swap Chain
3.  :heavy_check_mark: Vertex and Index Buffers
4.  :heavy_check_mark: Shaders and Pipeline Setup
5.  :heavy_check_mark: Rendering a Triangle
6.  :heavy_check_mark: Vertex Buffers
7.  :heavy_check_mark: Fragment Interpolation
8.  :heavy_check_mark: Vulkan Push Constant
9.  :heavy_check_mark: Rendering Systems
10. :heavy_check_mark: 2D Transformations
11. :heavy_check_mark: Euler Angles and Homogeneous Coordinate systems
12. :heavy_check_mark: Perspective Projection & Projection Matrix & Camera View
12. :heavy_check_mark: Simple Main Loop Pattern & User Input
13. :heavy_check_mark: Add Mouse Controls
14. :heavy_check_mark: Index and Staging buffers
15. :heavy_check_mark: Loading 3D Models
16. :heavy_check_mark: Replace World Objects with an ECS
17. :heavy_check_mark: Diffuse Shading
18. :heavy_check_mark: Move mouse controls in its own instance away from Keyboard controller and support simultenous mouse and keyboard movement
19. :heavy_check_mark: Completly delete WorldObjects and replace it with the ECS 
20. :construction: Uniform Buffers
21. :construction_worker: Descriptor Set

For even later:
- :construction: Add Quaternion support on Camera (functionnal code commented present but not fully implemented)

Please note that this roadmap is a general guideline and can be adjusted based of my preferences and project requirements. Each feature builds upon the previous ones, allowing me to incrementally expand the engine's capabilities while deepening my understanding of the underlying concepts and technologies.

## Getting Started

To get started with the engine, follow these steps:

1. Clone the repository: `git clone git@github.com:60-de-QI/vulkan-engine.git`
2. Install [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)
3. Install [Conan](https://conan.io/)
4. Run the Conan profile discovery tool `conan profile detect` to set the correct conan profile for your machine.
5. Conan will be able to build the necessary tools and dependencies (short of Vulkan) in the `conan-build` folder with the `conan install . --build=missing` command.
6. You should have a `CMakeUserPresets.json` by now. This file will be used by your Conan CMakePresets setting which you can select in the active build preset. All files will be built in the `conan-build/source/**/[mode]` folder with

For more detailed instructions on building and running the engine, please refer to the documentation or the individual source files.

### Adding a debugger in VSCode
1. Install the appropriate extension. We recommend the [LLDB debuger](https://marketplace.visualstudio.com/items?itemName=vadimcnvscode-lldb)
2. Make sure you have a launch.json in your `.vscode` folder.
3. Specify the generated example as the entry point for the debugger in your `launch.json` file, for debug on windows for example: `"program": "${workspaceFolder}/conan-build/source/example/Debug/vulqian_example.exe"`
4. Press F5 and debug should run!

## Requirements

Ensure you have the `VULKAN_SDK` environment variable set or compilation will not be able to find the SDK.
The engine uses this path to load both the Lib and the Includes.

[![Requirements](https://skillicons.dev/icons?i=cpp,cmake,git,vscode,v&perline=5)](https://github.com/60-de-QI/VulQIan)

## Contributing

Contributions to the project are welcome! If you find any bugs, have feature suggestions, or want to contribute improvements, please open an issue or submit a pull request.

## License

This project is licensed under the [MIT License](LICENSE).
