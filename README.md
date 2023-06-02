# Simple 3D Engine

This is a simple 3D engine built using Vulkan, C++, CMake, and Visual Studio Code. The engine aims to provide a foundation for creating real-time 3D applications.

## Features

- Window creation and Vulkan initialization.
- Vulkan swap chain for image presentation.
- Vertex and index buffers for geometry storage.
- Shaders and pipeline setup for rendering.
- Rendering of basic primitives (e.g., triangles).
- Uniform buffers for passing data to shaders.
- Texture loading and sampling in shaders.
- Basic lighting support using techniques like Phong shading.
- Model loading from common 3D formats (e.g., .obj).
- Camera controls and user interaction for scene navigation.
- Scene graph or entity-component system for scene organization.
- Advanced lighting and shading techniques (e.g., normal mapping, PBR).
- Shadow mapping for casting and receiving shadows.
- Post-processing effects like bloom and depth of field.
- Optimization techniques (e.g., frustum culling, LOD).
- Advanced rendering techniques (e.g., deferred shading, GPU compute).
- User interface system for displaying menus and UI elements.
- Sound and audio effects integration.
- Physics simulation using a physics library.
- Tooling and editor for content creation and scene management.

## Roadmap

1.  :construction: Window Creation and Vulkan Initialization
2.  :construction: Vulkan Swap Chain
3.  :construction: Vertex and Index Buffers
4.  :construction: Shaders and Pipeline Setup
5.  :construction: Rendering a Triangle
6.  :construction: Uniform Buffers
7.  :construction: Textures
8.  :construction: Basic Lighting
9.  :construction: Model Loading
10.  :construction: Camera Controls
11.  :construction: Scene Graph
12.  :construction: Advanced Lighting and Shading
13.  :construction: Shadow Mapping
14.  :construction: Post-Processing Effects
15.  :construction: Optimization and Performance
16.  :construction: Advanced Rendering Techniques
17.  :construction: User Interface
18.  :construction: Sound and Audio
19.  :construction: Physics Simulation
20.  :construction: Tooling and Editor

Please note that this roadmap is a general guideline and can be adjusted based on your preferences and project requirements. Each feature builds upon the previous ones, allowing you to incrementally expand your engine's capabilities while deepening your understanding of the underlying concepts and technologies.

## Getting Started

To get started with the engine, follow these steps:

1. Clone the repository: `git clone git@github.com:60-de-QI/vulkan-engine.git`
2. Install the necessary dependencies (Vulkan SDK, CMake, etc.).
3. Build the project using CMake: `cd engine && mkdir build && cd build && cmake .. && cmake --build .` or use the `CMakePresets.json` file for vscode containers.
4. Run the executable generated by the build process.
5. Explore the codebase and start implementing features following the roadmap.

For more detailed instructions on building and running the engine, please refer to the documentation or the individual source files.

## Requirements

[![My Skills](https://skillicons.dev/icons?i=cpp,cmake,git,vscode,v&perline=5)](https://skillicons.dev)

## Contributing

Contributions to the project are welcome! If you find any bugs, have feature suggestions, or want to contribute improvements, please open an issue or submit a pull request.

## License

This project is licensed under the [MIT License](LICENSE).
