# NeKoRoGEAR

Pronounced as "necro-gear." It is a generic general-purpose 3D game engine I made for personal use and is by no means professional or production-ready (yet)! Feel free to use as template or modify according to your liking.
<br>
<br>

## Project Goals
- Practice and understand the concepts of C++.
- Make a simple general-purpose game engine with a working game example.
  - Achieve a Source-esque gameplay vibe, and have similar toolsets like an equivalent of Hammer for level editing but for this engine.
- Understand game development at a deeper level with minimal help from an LLM (strictly help for documentations and code snippets only!).
- Adapt better coding practices for more optimal, efficient, and readable code. Including but not limited to:
  - Abstraction
  - Decoupling
  - Single Responsibility
<br>

## Core Tech Stack
The very foundation of the engine. As much as I want to reinvent the wheel to further deepen my understanding about the inner workings of game engines, I settled with using established libraries when making the core of the engine.
- C++ as main programming language
- C# via hostfxr for client and gameplay logic
- CMake for build automation
- SDL3 for window rendering and inputs
- OpenGL3 via GLAD for graphics
- stb_image for asset loading
- Jolt Physics for physics simulations
- ImGui for Debug/Editor menus
- GLM for math calculations
- miniaudio/FMOD for sounds
<br>

```
src
├── Core
│    ├── Assets
│    ├── Components
│    ├── ECS
│    ├── Events
│    └── Rendering
├── CSharp
│    └── WIP
├── Editor
│    └── WIP
└── Runtime
     └── WIP
```

<br>

## Planned Custom Features
These are the systems and features of my engine that are quite specific and I deem to be worth manually making on top of the core. It's what makes NeKoRoGEAR somewhat its own thing! :D
- Rendering Hardware Interface - NeKoRoGEAR has an abstraction layer for graphics and rendering which sits between OpenGL and the engine core. As this engine grows, I'd rather avoid having nightmares refactoring the rendering engine just to support another graphics library like Vulkan.
- Event System - Inputs and window events.
  - Dedicated Input Manager - Similar to Unity Input System.
- Entity Component System - I want to stray from Unity's OOP/GameObject workflow because Entities and Components are more performant.
- Editor and Game clients - The architecture for NeKoRoGEAR was built so that any client based on the engine shares the same library.
- Scene Management
  - Load external C# scripts, meshes, shaders, and textures.
<br>

## TODO
- Implement the rest of the tech stack.
- Prototype the essentials:
  - Test scene
  - Camera view
  - Player Movement
  - Custom gameplay mechanics
- Documentation. At the time I am writing this, only God knows whatever the hell I'm doing and I pray everyday that I remember how things go within the project.
