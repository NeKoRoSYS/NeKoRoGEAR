#pragma once
#include <variant>
#include <SDL3/SDL_keycode.h>

struct WindowCloseEvent {};

struct WindowResizeEvent {
    int width, height;
};

struct KeyPressEvent {
    SDL_Keycode key;
    bool repeat;
};

using EngineEvent = std::variant<
    WindowCloseEvent, 
    WindowResizeEvent, 
    KeyPressEvent
>;