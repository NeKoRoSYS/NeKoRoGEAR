#pragma once
#include <cstdint>

enum class KeyCode : uint16_t {
    Unknown = 0,
    Num0 = 1,
    Num1 = 2,
    Num2 = 3,
    Num3 = 4,
    Num4 = 5,
    Num5 = 6,
    Num6 = 7,
    Num7 = 8,
    Num8 = 9,
    Num9 = 10,
    A = 11,
    B = 12,
    C = 13,
    D = 14,
    E = 15,
    F = 16,
    G = 17,
    H = 18,
    I = 19,
    J = 20,
    K = 21,
    L = 22,
    M = 23,
    N = 24,
    O = 25,
    P = 26,
    Q = 27,
    R = 28,
    S = 29,
    T = 30,
    U = 31,
    V = 32,
    W = 33,
    X = 34,
    Y = 35,
    Z = 36,
    Space = 37,
    LShift = 38,
    LControl = 39
};

enum class MouseButton : uint8_t {
    Left = 0,
    Right = 1,
    Middle = 2
};

enum class MouseAxis : uint8_t {
    X = 0,
    Y = 1,
    Scroll = 2
};

enum class InputAxis : uint32_t {
    MoveForward = 0,
    MoveRight = 1,
    LookUp = 2,
    LookRight = 3,
    MoveUp = 4
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