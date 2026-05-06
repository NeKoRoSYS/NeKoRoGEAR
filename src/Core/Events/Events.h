#pragma once
#include <variant>
#include <SDL3/SDL_keycode.h>
#include <Core/Components/InputCodes.h>

struct WindowCloseEvent {};

struct WindowResizeEvent {
    int width, height;
};

struct KeyPressEvent {
    SDL_Keycode key;
    bool pressed;
    bool repeat;
};

struct MouseButtonEvent {
    uint8_t button;
    bool pressed;
};

struct MouseMotionEvent { float xRel, yRel; };
struct ActionEvent { InputAction action; InputState state; };
struct AxisEvent { InputAxis axis; float value; };

using EngineEvent = std::variant<
    WindowCloseEvent, 
    WindowResizeEvent, 
    KeyPressEvent,
    MouseButtonEvent,
    MouseMotionEvent,
    ActionEvent,
    AxisEvent
>;