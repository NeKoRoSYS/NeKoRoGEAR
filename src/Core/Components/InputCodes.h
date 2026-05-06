#pragma once
#include <cstdint>

enum class KeyCode : uint16_t {
    Unknown = 0,
    W, A, S, D, Q, E, Space, LShift, LControl
};

enum class MouseButton : uint8_t {
    Left, Right, Middle
};

enum class MouseAxis : uint8_t {
    X, Y, Scroll
};

enum class InputAxis : uint32_t {
    MoveForward, 
    MoveRight,   
    LookUp,      
    LookRight,    
    MoveUp
};

enum class InputAction : uint32_t {
    Sprint,
    ToggleCameraMode,
    PanCamera
};

enum class InputState {
    None,
    Pressed,
    Held,
    Released
};