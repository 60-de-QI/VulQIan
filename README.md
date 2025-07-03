# VulQIan: The "Epic" Intelligence Engine
[![CMake](https://github.com/60-de-QI/VulQIan/actions/workflows/cmake.yml/badge.svg)](https://github.com/60-de-QI/VulQIan/actions/workflows/cmake.yml)

<p align="center">
  <img width="796" height="594" src="https://github.com/60-de-QI/VulQIan/blob/main/.github/VulQIan.gif">
</p>
<p align="center">
  <img width="803" height="641" src="https://github.com/60-de-QI/VulQIan/blob/main/.github/alpha_blending.png">
  <br>
  <em>Demonstrating advanced alpha blending with depth-sorted transparency</em>
</p>

VulQIan is a modern 3D graphics engine built using Vulkan, C++20, CMake and Visual Studio Code. The engine provides a robust foundation for creating real-time 3D applications with advanced rendering features. This project serves as an exploration of low-level graphics programming concepts and modern rendering techniques.

## Features

### Core Engine
- **Vulkan-based rendering** with modern graphics pipeline
- **Entity Component System (ECS)** architecture for efficient scene management
- **Window creation** and Vulkan initialization with swap chain support
- **Advanced memory management** with vertex, index, and uniform buffers
- **Modular render systems** with proper pipeline management

### Rendering System
- **Alpha blending and transparency** with sophisticated depth sorting
- **Multiple lighting models** including diffuse and specular lighting
- **Dynamic point light system** with real-time positioning and colors
- **Billboard rendering** for efficient light visualization
- **3D model loading** and rendering with multiple format support
- **Mixed transparent/opaque rendering** with optimal performance

### Scene Management
- **3D transformations** (translation, rotation, scaling) with matrix operations
- **Camera system** with perspective projection and view controls
- **Input handling** with simultaneous mouse and keyboard controls
- **Real-time animation** and dynamic object movement
- **Efficient culling** and rendering optimization

### Graphics Features
- **Fragment interpolation** and advanced lighting calculations
- **Push constants** for efficient shader data transfer
- **Shader compilation** and pipeline setup automation
- **Depth testing** with proper Z-buffer management
- **Multi-pass rendering** for complex visual effects

### Advanced Rendering Features

#### Alpha Blending & Transparency
VulQIan supports sophisticated transparency rendering with:
- **Depth-sorted transparency**: Objects are automatically sorted by distance from camera
- **Mixed opaque/transparent rendering**: Opaque objects render first, followed by transparent objects back-to-front
- **Integrated light transparency**: Point lights and transparent meshes are rendered in correct depth order
- **Component-based transparency**: Simple `Transparency` component with configurable alpha and color tint

The transparency system ensures proper visual layering where lights behind transparent objects appear dimmed, while maintaining optimal rendering performance through careful depth sorting.

## Quick Start Example

### ECS Usage
```cpp
// Create an entity with mesh and transparency
auto entity = coordinator.create_entity();

// Add transform component
Vulqian::Engine::ECS::Components::Transform_TB_YXZ transform{};
transform.translation = glm::vec3{0.0f, 0.0f, -2.0f};
transform.scale = glm::vec3{2.0f, 2.0f, 1.0f};

// Add mesh component
Vulqian::Engine::ECS::Components::Mesh mesh{};
mesh.model = Vulqian::Engine::Graphics::Model::create_model_from_file(device, "quad.obj");

// Add transparency component
Vulqian::Engine::ECS::Components::Transparency transparency{};
transparency.alpha = 0.5f;
transparency.color = glm::vec3{1.0f, 0.0f, 0.0f}; // Red tint

// Attach components to entity
coordinator.add_component(entity, transform);
coordinator.add_component(entity, mesh);
coordinator.add_component(entity, transparency);
```

## Roadmap

1.  :heavy_check_mark: Window Creation and Vulkan Initialization
2.  :heavy_check_mark: Vulkan Swap Chain
3.  :heavy_check_mark: Vertex and Index Buffers
4.  :heavy_check_mark: Shaders and Pipeline Setup
5.  :heavy_check_mark: Rendering a Triangle
6.  :heavy_check_mark: Vertex Buffers
7.  :heavy_check_mark: Fragment Interpolation
8.  :heavy_check_mark: Vulkan Push Constants
9.  :heavy_check_mark: Rendering Systems
10. :heavy_check_mark: 2D Transformations
11. :heavy_check_mark: Euler Angles and Homogeneous Coordinate Systems
12. :heavy_check_mark: Perspective Projection & Projection Matrix & Camera View
12. :heavy_check_mark: Simple Main Loop Pattern & User Input
13. :heavy_check_mark: Add Mouse Controls
14. :heavy_check_mark: Index and Staging Buffers
15. :heavy_check_mark: Loading 3D Models
16. :heavy_check_mark: Replace World Objects with an ECS
17. :heavy_check_mark: Diffuse Shading
18. :heavy_check_mark: Move mouse controls in its own instance away from Keyboard controller and support simultaneous mouse and keyboard movement
19. :heavy_check_mark: Completely delete WorldObjects and replace it with the ECS 
20. :heavy_check_mark: Uniform Buffers
21. :heavy_check_mark: Unit Testing support with Google Tests
22. :heavy_check_mark: Descriptor Sets
23. :heavy_check_mark: Point Lights
24. :heavy_check_mark: Fragment Lighting
25. :heavy_check_mark: Billboards
26. :heavy_check_mark: Multiple Light Systems
27. :heavy_check_mark: Specular Lighting
28. :heavy_check_mark: Alpha Blending & Transparency
29. :construction: Image and Texture Mapping

### Future Enhancements:
- :construction: Add Quaternion support on Camera (functional code commented present but not fully implemented)
- :construction: Implement single-pass light rendering instead of using push constants for better performance with large numbers of lights
- :construction: Advanced shadow mapping
- :construction: Post-processing effects pipeline
- :construction: Physically Based Rendering (PBR)

Please note that this roadmap is a general guideline and can be adjusted based on project requirements and priorities. Each feature builds upon the previous ones, allowing for incremental expansion of the engine's capabilities while deepening understanding of underlying graphics programming concepts.

## Getting Started

To get started with VulQIan, follow these steps:

1. **Clone the repository**: `git clone git@github.com:60-de-QI/vulkan-engine.git`
2. **Install [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)**
3. **Install [Conan](https://conan.io/)**
4. **Run the Conan profile discovery tool** `conan profile detect` to set the correct conan profile for your machine
5. **Build dependencies** with Conan using `conan install . --build=missing` command (this creates the `conan-build` folder)
6. **Configure CMake** - You should have a `CMakeUserPresets.json` by now. Select the appropriate Conan CMakePresets setting in your build preset
7. **Build the project** - All files will be built in the `conan-build/source/**/[mode]` folder

For more detailed instructions on building and running the engine, please refer to the documentation or the individual source files.

### Adding a Debugger in VSCode
1. **Install the appropriate extension** - We recommend the [LLDB debugger](https://marketplace.visualstudio.com/items?itemName=vadimcnvscode-lldb)
2. **Ensure you have a launch.json** in your `.vscode` folder
3. **Specify the generated example** as the entry point for the debugger in your `launch.json` file
   - For debug on Windows: `"program": "${workspaceFolder}/conan-build/source/example/Debug/vulqian_example.exe"`
4. **Press F5** and debug should run!

### Understanding the ECS Architecture

As of Step 19 in the roadmap, VulQIan incorporates a complete Entity Component System (ECS) architecture. This integration allows for efficient and scalable management of entities within the 3D engine.

The ECS design pattern in VulQIan revolves around four main elements:

- **Entities**: Basic elements representing game objects or entities that need to be managed
- **Components**: Data elements attached to entities, defining their behavior or characteristics (position, velocity, appearance, transparency, etc.)
- **Systems**: Logic or behavior applied to groups of entities that possess specific combinations of components
- **Coordinator**: Meta object that preserves the purity of the ECS pattern by managing interactions between entities, components, and systems

All ECS code is contained within `source/VulQIan/ECS` and you can see practical usage examples in `void App::load_entities()` in `source/examples/App.cpp`, which demonstrates mesh systems, transformation systems, and transparency components.

## Requirements

Ensure you have the `VULKAN_SDK` environment variable set or compilation will not be able to find the SDK. The engine uses this path to load both the libraries and includes.

[![Requirements](https://skillicons.dev/icons?i=cpp,cmake,git,vscode&perline=5)](https://github.com/60-de-QI/VulQIan)
<img src="https://github.com/60-de-QI/VulQIan/assets/114400741/831bb526-e430-4621-9ccd-e0e03b5cc3ee" data-canonical-src="https://github.com/60-de-QI/VulQIan/assets/114400741/831bb526-e430-4621-9ccd-e0e03b5cc3ee" width="75" />

- **C++20** is recommended as it's the main language this engine uses
- **CMake** is the build system and generator used in the project
- **VSCode** is not required, but recommended for smooth integration with Conan and the CMake building process
- **Vulkan API** is required to run the engine on your machine

## CMake Configuration and Options

Through CMake you can toggle certain options and configure the repo at a higher level. You can check these options in the `cmake/options.cmake` file. This directory is also where you can tune CMake to your preferences.

| Option | Values | Description |
| ------ | ------ | ----------- |
| `VULQIAN_BUILD_EXAMPLE` | ON/OFF | Builds the default demo application showcasing engine capabilities (located in `source/examples/`) |
| `ENABLE_SHADER_COMPILATION` | ON/OFF | Enables automatic shader compilation using scripts in `source/VulQIan/Shaders` with `glslc` |
| `VULQIAN_BUILD_TESTS` | ON/OFF | Builds unit tests in the `source/VulQIan/tests` directory for use with CTest. Toggling OFF completely bypasses the unit testing phase |

## Performance Notes

VulQIan is designed with performance in mind:
- **Efficient depth sorting** for transparent objects minimizes overdraw
- **Component-based architecture** allows for cache-friendly data access patterns
- **Minimal state changes** through careful pipeline management
- **Optimized render loops** that separate opaque and transparent rendering passes

## Contributing

Contributions to the project are welcome! If you find any bugs, have feature suggestions, or want to contribute improvements, please open an issue or submit a pull request.

When contributing:
- Follow the existing code style and architecture patterns
- Add unit tests for new features when applicable
- Update documentation for significant changes
- Test your changes across different platforms if possible

## License

This project is licensed under the [MIT License](LICENSE).
