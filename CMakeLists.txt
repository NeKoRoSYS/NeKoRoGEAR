cmake_minimum_required(VERSION 3.20)
project(NeKoRoGEAR VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

add_library(stb_image STATIC vendor/stb/stb_image.cpp)
target_include_directories(stb_image PUBLIC vendor/stb)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vendor/SDL3")
find_package(SDL3 REQUIRED)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) 
add_subdirectory(vendor/assimp)

set(IMGUI_SOURCES
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/imgui_impl_sdl3.cpp
    vendor/imgui/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC vendor/imgui)
target_link_libraries(imgui PRIVATE SDL3::SDL3)

set(ENGINE_SOURCES
    src/Core/Components/Application.cpp
    src/Core/Components/DebugMenu.cpp
    src/Core/Components/InputManager.cpp
    src/Core/Components/SceneSerializer.cpp
    src/Core/Rendering/Renderer.cpp
    src/Core/Rendering/Shader.cpp
    src/Core/Rendering/Window.cpp
    src/Core/Rendering/Buffer.cpp
    src/Core/Rendering/RendererAPI.cpp
    src/Core/Rendering/OpenGL/OpenGLBuffer.cpp
    src/Core/Rendering/OpenGL/OpenGLRenderCommand.cpp
    src/Core/Rendering/OpenGL/OpenGLTexture.cpp
)

add_library(NeKoRoGEAR ${ENGINE_SOURCES})

target_include_directories(NeKoRoGEAR PUBLIC src vendor)

target_link_libraries(NeKoRoGEAR PUBLIC
    glad
    stb_image
    imgui
    SDL3::SDL3
    assimp 
)

add_executable(nGEAR_EDITOR src/Editor/Editor.cpp)
target_link_libraries(nGEAR_EDITOR PRIVATE NeKoRoGEAR)

add_executable(nGEAR_RUNTIME src/Runtime/Runtime.cpp)
target_link_libraries(nGEAR_RUNTIME PRIVATE NeKoRoGEAR)


if (WIN32)
    add_custom_command(TARGET nGEAR_EDITOR POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/vendor/SDL3/lib/x64/SDL3.dll"
        "$<TARGET_FILE_DIR:nGEAR_EDITOR>"
        COMMENT "Copying SDL3.dll to the output directory..."
    )
    
    add_custom_command(TARGET nGEAR_RUNTIME POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/vendor/SDL3/lib/x64/SDL3.dll"
        "$<TARGET_FILE_DIR:nGEAR_RUNTIME>"
        COMMENT "Copying SDL3.dll to the output directory..."
    )
endif()