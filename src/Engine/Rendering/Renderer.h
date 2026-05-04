#pragma once

#include "Window.h"

class Renderer {
public:
    Renderer(Window& window);
    void Clear();
    float clearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
};