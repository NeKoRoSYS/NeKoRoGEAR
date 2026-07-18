# NeKoRoGEAR

Pronounced as "necro-gear." It is a generic general-purpose 3D game engine I made for personal use and is by NO MEANS professional or production-ready (yet)! Feel free to use as template or modify according to your liking.
<br>
<br>

## Project Goals
- Practice and understand the concepts of C++.
- Make a simple general-purpose game engine with a working game example.
  - Achieve a Source-esque gameplay vibe, and have similar toolsets like an equivalent of Hammer for level editing but for this engine.
- Understand game development at a deeper level with minimal help from an LLM (strictly help for documentations and code snippets only!).
- Adapt better coding practices for more optimal, efficient, and readable code. Including but not limited to:
  - Abstraction
  - Decoupling/Separation of Concerns
  - Single Responsibility
<br>

## Core Tech Stack
The very foundation of the engine. As much as I want to reinvent the wheel to further deepen my understanding about the inner workings of game engines, I settled with using established libraries when making the core of the engine because I just want a safe and working solution for myself without ever having to be admitted to the psych ward.
- [C++](https://isocpp.org/) as main programming language
- [C#](https://dotnet.microsoft.com/en-us/languages/csharp) via [hostfxr](https://github.com/dotnet/runtime/blob/main/src/native/corehost/hostfxr.h) for client and gameplay logic
- [CMake](https://github.com/kitware/cmake) for build automation
- [SDL3](https://github.com/libsdl-org/SDL) for window rendering and inputs
- [OpenGL3](https://www.opengl.org/) via [glad](https://github.com/dav1dde/glad) for graphics
- [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) for asset loading
- [Jolt Physics](https://github.com/jrouwe/JoltPhysics) for physics simulations
- [ImGui](https://github.com/ocornut/imgui) for Debug/Editor menus
- [glm](https://github.com/g-truc/glm/releases) for math calculations
- [miniaudio](https://github.com/mackron/miniaudio)/[FMOD](https://www.fmod.com/) for sounds
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
