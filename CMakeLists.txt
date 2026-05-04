cmake_minimum_required(VERSION 3.20)

project(NeKoRoGINE VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SDL3_DIR ${CMAKE_SOURCE_DIR}/vendor/SDL3)
set(SDL3_INCLUDE_DIR ${SDL3_DIR}/include)
set(SDL3_LIB_DIR ${SDL3_DIR}/lib/x64)
set(SDL3_LIB ${SDL3_LIB_DIR}/SDL3.lib)
set(SDL3_DLL ${SDL3_LIB_DIR}/SDL3.dll)

set(GLAD_DIR ${CMAKE_SOURCE_DIR}/vendor/glad)
set(GLAD_INCLUDE_DIR ${GLAD_DIR}/include)
set(GLAD_SOURCE ${GLAD_DIR}/src/glad.c)

set(GLM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/vendor/glm)

set(STB_DIR ${CMAKE_SOURCE_DIR}/vendor/stb)
set(STB_IMAGE_INCLUDE ${GLAD_DIR}/stb_image.h)
set(STB_IMAGE_SOURCE ${STB_DIR}/stb_image.cpp)

set(IMGUI_SOURCES
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/imgui_impl_sdl3.cpp
    vendor/imgui/imgui_impl_opengl3.cpp
)

file(GLOB_RECURSE ENGINE_GLOB "src/*.cpp" "src/*.h")
set(ENGINE_SOURCES
    src/Engine/main.cpp
    ${ENGINE_GLOB}
    ${GLAD_SOURCE}
    ${IMGUI_SOURCES}
    ${STB_IMAGE_SOURCE}
)

add_executable(NeKoRoGINE ${ENGINE_SOURCES} )

target_include_directories(NeKoRoGINE PRIVATE
    vendor/imgui
    ${SDL3_INCLUDE_DIR}
    ${GLAD_INCLUDE_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(NeKoRoGINE PRIVATE ${SDL3_LIB})

set_target_properties(NeKoRoGINE PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

add_custom_command(TARGET NeKoRoGINE POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SDL3_DLL}"
    "$<TARGET_FILE_DIR:NeKoRoGINE>"
    COMMENT "Copying SDL3.dll to the output directory..."
)