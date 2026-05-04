#include "Renderer.h"
#include "Window.h"
#include <glad/glad.h>
#include <iostream>

Renderer::Renderer(Window& window) {
    if (!gladLoadGLLoader((GLADloadproc)window.GetProcAddress())) {
        throw std::runtime_error("Failed to initialize GLAD");
    }
}

void Renderer::Clear() {
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}