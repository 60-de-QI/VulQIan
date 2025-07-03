# VulQIan: The Epic Intelligence Engine
[![CMake](https://github.com/60-de-QI/VulQIan/actions/workflows/cmake.yml/badge.svg)](https://github.com/60-de-QI/VulQIan/actions/workflows/cmake.yml)

<p align="center">
  <img width="800" height="600" src="https://github.com/60-de-QI/VulQIan/blob/main/.github/VulQIan.gif">
</p>

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
20. :heavy_check_mark: Uniform Buffers
21. :heavy_check_mark: Unit Testing support with google tests.
22. :heavy_check_mark: Descriptor Set
23. :heavy_check_mark: Point Lights
24. :heavy_check_mark: Fragment Lighting
25. :heavy_check_mark: Billboards
26. :heavy_check_mark: Multiple Light Systems
27. :heavy_check_mark: Specular Lighting
28. :heavy_check_mark: Alpha Blending & Transparency
29. :construction: Image and Texture Mapping

For even later:
- :construction: Add Quaternion support on Camera (functionnal code commented present but not fully implemented)
- :construction: Do a single pass for rendering lights instead of using push constants, this will allow to render a great number of lights efficiently

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

### More on the ECS
As of the Replace World Objects with an ECS feature (completed in Step 19 of the roadmap), VulQIan now incorporates an Entity Component System (ECS) architecture. This integration allows for more efficient and scalable management of entities within the 3D engine.

The ECS design pattern in VUlQIan revolves around three main components and one meta component:

- **Entities:** These are the basic elements of the ECS pattern, representing the game objects or entities that need to be managed.
- **Components:** These are the data elements attached to entities, defining their behavior or characteristics. Components can be simple or complex, representing attributes like position, velocity, appearance, or any other aspect of an entity.
- **Systems:** Systems are the logic or behavior applied to groups of entities that possess specific combinations of components. Systems act on entities with specific component configurations, allowing for efficient and modular processing.
- **Coordinator:** This is a meta object that serves only to preserve the rest of the other objects from beeing polluted by logic systems that are incompatible with the aim of offering a pure ECS pattern to the user.

All the ECS code is contained within: `source/VulQIan/ECS` and you can check the example App to see its usage in action with components that offer mesh systems and transformation systems in `void App::load_entities()` in `source/examples/App.cpp`.

## Requirements

Ensure you have the `VULKAN_SDK` environment variable set or compilation will not be able to find the SDK.
The engine uses this path to load both the Lib and the Includes.

[![Requirements](https://skillicons.dev/icons?i=cpp,cmake,git,vscode&perline=5)](https://github.com/60-de-QI/VulQIan)
<img src="https://github.com/60-de-QI/VulQIan/assets/114400741/831bb526-e430-4621-9ccd-e0e03b5cc3ee" data-canonical-src="https://github.com/60-de-QI/VulQIan/assets/114400741/831bb526-e430-4621-9ccd-e0e03b5cc3ee" width="75" />
- C++ is recommend as its the main language this engine uses
- CMake is the tooling and generator used in the project
- VsCode is not required, but recommended for a smooth integration with Conan and the building process with CMake
- The Vulkan API is required for you to run the engine on your machine.

## CMake and Options
Through CMake you can toggle certain options and use the repo at a higher level. You can check these options in the cmake/options.cmake file.
This directory is also where you can tune CMake to your liking.

| Value  | Option | Detail  |
| ------------- | ------------- | ------------- |
| VULQIAN_BUILD_EXAMPLE  | ON/OFF  | This is the default app to try and see the capabilities of the engine located by default in `source/examples/` |
| ENABLE_SHADER_COMPILATION  | ON/OFF  | A .sh and .bat script is located in `source/VulQIan/Shaders` where you can use `glslc` to compile your .vert and .frag |
| VULQIAN_BUILD_TESTS | ON/OFF | builds the unit tests in the `source/VulQIan/tests` directory. They can be used automatically by Ctests. Toggling this option off completly bypasses the unit testing phase. |

## Contributing

Contributions to the project are welcome! If you find any bugs, have feature suggestions, or want to contribute improvements, please open an issue or submit a pull request.

## License

This project is licensed under the [MIT License](LICENSE).
