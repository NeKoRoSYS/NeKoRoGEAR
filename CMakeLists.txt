cmake_minimum_required(VERSION 3.20)
project(NeKoRoGINE VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Static Libraries ---
add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

add_library(stb_image STATIC vendor/stb/stb_image.cpp)
target_include_directories(stb_image PUBLIC vendor/stb)

# --- Packages ---
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vendor/SDL3")
find_package(SDL3 REQUIRED)

# --- ASSIMP SUBMODULE CONFIGURATION ---
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) 
add_subdirectory(vendor/assimp)

# --- ImGui Setup ---
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

# --- Engine Executable ---
set(ENGINE_SOURCES
    src/Engine/main.cpp
    src/Engine/Components/Application.cpp
    src/Engine/Components/DebugMenu.cpp
    src/Engine/Rendering/Renderer.cpp
    src/Engine/Rendering/Shader.cpp
    src/Engine/Rendering/Window.cpp
    src/Engine/Rendering/Buffer.cpp
    src/Engine/Rendering/OpenGL/OpenGLBuffer.cpp
    src/Engine/Rendering/OpenGL/OpenGLRenderCommand.cpp
    src/Engine/Rendering/OpenGL/OpenGLTexture.cpp
)

add_executable(NeKoRoGINE ${ENGINE_SOURCES})

target_include_directories(NeKoRoGINE PRIVATE
    src
    vendor
)

# --- Target Linking ---
target_link_libraries(NeKoRoGINE PRIVATE
    glad
    stb_image
    imgui
    SDL3::SDL3
    assimp 
)

# --- Output Properties ---
set_target_properties(NeKoRoGINE PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

# --- Post-Build ---
if (WIN32)
    add_custom_command(TARGET NeKoRoGINE POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/vendor/SDL3/lib/x64/SDL3.dll"
        "$<TARGET_FILE_DIR:NeKoRoGINE>"
        COMMENT "Copying SDL3.dll to the output directory..."
    )
endif()