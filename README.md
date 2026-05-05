# NeKoRoGINE

A generic game engine I made as a hobby and is by no means production-ready! Feel free to use as template or modify idk
<br>

## Core Tech Stack
The very foundation of the engine. As much as I want to reinvent the wheel to further deepen my understanding about the inner workings of game engines, I settled for established libraries when making the core of the engine.
- C++ as main programming language
- C# for client and gameplay logic
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
└── Engine
     ├── Assets
     ├── Components
     ├── ECS
     ├── Events
     └── Rendering
└── CSharp
     └── WIP
└── Game
     └── WIP
```

<br>

## Custom Features
These are the systems and features of my engine that are quite specific and I deem to be worth manually making on top of the core. It's what makes NeKoRoGINE somewhat its own thing! :D
- Rendering Hardware Interface - as I'm writing this, Minecraft got its Vulcan API support along the already-established OpenGL. As this engine grows, I'd rather avoid having nightmares refactoring the rendering engine just to support another graphics library.
- Event system - Inputs and window events.
- Entity Component System - I want to stray from Unity's OOP/GameObject workflow because Entities and Components are more performant.
- Load external C# scripts, meshes, shaders, and textures.
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

## TODO
- Implement the rest of the tech stack.
- Prototype the essentials:
  - Test scene
  - Camera view
  - Player Movement
  - Custom gameplay mechanics
