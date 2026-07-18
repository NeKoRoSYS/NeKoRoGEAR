#include "InputManager.h"
#include <SDL3/SDL.h>

static KeyCode MapSDLToEngineKey(SDL_Keycode key) {
    switch (key) {
        case SDLK_A: return KeyCode::A;
        case SDLK_B: return KeyCode::B;
        case SDLK_C: return KeyCode::C;
        case SDLK_D: return KeyCode::D;
        case SDLK_E: return KeyCode::E;
        case SDLK_F: return KeyCode::F;
        case SDLK_G: return KeyCode::G;
        case SDLK_H: return KeyCode::H;
        case SDLK_I: return KeyCode::I;
        case SDLK_J: return KeyCode::J;
        case SDLK_K: return KeyCode::K;
        case SDLK_L: return KeyCode::L;
        case SDLK_M: return KeyCode::M;
        case SDLK_N: return KeyCode::N;
        case SDLK_O: return KeyCode::O;
        case SDLK_P: return KeyCode::P;
        case SDLK_Q: return KeyCode::Q;
        case SDLK_R: return KeyCode::R;
        case SDLK_S: return KeyCode::S;
        case SDLK_T: return KeyCode::T;
        case SDLK_U: return KeyCode::U;
        case SDLK_V: return KeyCode::V;
        case SDLK_W: return KeyCode::W;
        case SDLK_X: return KeyCode::X;
        case SDLK_Y: return KeyCode::Y;
        case SDLK_Z: return KeyCode::Z;
        case SDLK_SPACE: return KeyCode::Space;
        case SDLK_LSHIFT: return KeyCode::LShift;
        case SDLK_LCTRL: return KeyCode::LControl;
        default: return KeyCode::Unknown;
    }
}

static SDL_Scancode MapEngineKeyToSDL(KeyCode key) {
    switch (key) {
        case KeyCode::A: return SDL_SCANCODE_A;
        case KeyCode::B: return SDL_SCANCODE_B;
        case KeyCode::C: return SDL_SCANCODE_C;
        case KeyCode::D: return SDL_SCANCODE_D;
        case KeyCode::E: return SDL_SCANCODE_E;
        case KeyCode::F: return SDL_SCANCODE_F;
        case KeyCode::G: return SDL_SCANCODE_G;
        case KeyCode::H: return SDL_SCANCODE_H;
        case KeyCode::I: return SDL_SCANCODE_I;
        case KeyCode::J: return SDL_SCANCODE_J;
        case KeyCode::K: return SDL_SCANCODE_K;
        case KeyCode::L: return SDL_SCANCODE_L;
        case KeyCode::M: return SDL_SCANCODE_M;
        case KeyCode::N: return SDL_SCANCODE_N;
        case KeyCode::O: return SDL_SCANCODE_O;
        case KeyCode::P: return SDL_SCANCODE_P;
        case KeyCode::Q: return SDL_SCANCODE_Q;
        case KeyCode::R: return SDL_SCANCODE_R;
        case KeyCode::S: return SDL_SCANCODE_S;
        case KeyCode::T: return SDL_SCANCODE_T;
        case KeyCode::U: return SDL_SCANCODE_U;
        case KeyCode::V: return SDL_SCANCODE_V;
        case KeyCode::W: return SDL_SCANCODE_W;
        case KeyCode::X: return SDL_SCANCODE_X;
        case KeyCode::Y: return SDL_SCANCODE_Y;
        case KeyCode::Z: return SDL_SCANCODE_Z;
        case KeyCode::Space: return SDL_SCANCODE_SPACE;
        case KeyCode::LShift: return SDL_SCANCODE_LSHIFT;
        case KeyCode::LControl: return SDL_SCANCODE_LCTRL;
        default: return SDL_SCANCODE_UNKNOWN;
    }
}

static MouseButton MapSDLToEngineMouse(uint8_t button) {
    switch (button) {
        case SDL_BUTTON_LEFT: return MouseButton::Left;
        case SDL_BUTTON_RIGHT: return MouseButton::Right;
        case SDL_BUTTON_MIDDLE: return MouseButton::Middle;
        default: return MouseButton::Left;
    }
}

void InputManager::BindAction(InputAction action, KeyCode key) { keyActionBindings[key].push_back(action); }
void InputManager::BindAction(InputAction action, MouseButton button) { mouseActionBindings[button].push_back(action); }
void InputManager::BindAxis(InputAxis axis, KeyCode key, float scale) { keyAxisBindings[axis].push_back({key, scale}); }
void InputManager::BindAxis(InputAxis axis, MouseAxis rawAxis, float scale) { mouseAxisBindings[axis].push_back({rawAxis, scale}); }

void InputManager::Update() {
    currentMouseDeltas[MouseAxis::X] = 0.0f;
    currentMouseDeltas[MouseAxis::Y] = 0.0f;

    for (auto& [action, state] : actionStates) {
        if (state == InputState::Pressed) state = InputState::Held;
        if (state == InputState::Released) state = InputState::None;
    }
}

template<typename HardwareID>
void InputManager::ProcessHardwareState(HardwareID id, bool isPressed, EventBus& bus) {
    const auto& bindings = GetBindings(id);
    for (InputAction action : bindings) {
        InputState newState = isPressed ? InputState::Pressed : InputState::Released;
        actionStates[action] = newState;
        bus.Push(ActionEvent{action, newState});
    }
}

void InputManager::ProcessRawEvent(const EngineEvent& event, EventBus& outEventBus) {
    std::visit([this, &outEventBus](auto&& e) {
        using T = std::decay_t<decltype(e)>;
                 
        if constexpr (std::is_same_v<T, KeyPressEvent>) {
            if (!e.repeat) {
                ProcessHardwareState(MapSDLToEngineKey(e.key), e.pressed, outEventBus); 
            }
        } 
        else if constexpr (std::is_same_v<T, MouseButtonEvent>) {
            ProcessHardwareState(MapSDLToEngineMouse(e.button), e.pressed, outEventBus);
        }
        else if constexpr (std::is_same_v<T, MouseMotionEvent>) {
            currentMouseDeltas[MouseAxis::X] += e.xRel;
            currentMouseDeltas[MouseAxis::Y] += e.yRel;
        }
    }, event);
}

bool InputManager::IsActionActive(InputAction action) const {
    auto it = actionStates.find(action);
    return it != actionStates.end() && (it->second == InputState::Pressed || it->second == InputState::Held);
}

bool InputManager::WasActionJustPressed(InputAction action) const {
    auto it = actionStates.find(action);
    return it != actionStates.end() && it->second == InputState::Pressed;
}

float InputManager::GetAxisValue(InputAxis axis) const {
    float totalValue = 0.0f;

    if (auto it = keyAxisBindings.find(axis); it != keyAxisBindings.end()) {
        const bool* keys = SDL_GetKeyboardState(NULL);
        for (const auto& bind : it->second) {
            if (keys[MapEngineKeyToSDL(bind.key)]) totalValue += bind.scale;
        }
    }

    if (totalValue > 1.0f) totalValue = 1.0f;
    if (totalValue < -1.0f) totalValue = -1.0f;

    if (auto it = mouseAxisBindings.find(axis); it != mouseAxisBindings.end()) {
        for (const auto& bind : it->second) {
            if (auto mouseIt = currentMouseDeltas.find(bind.axis); mouseIt != currentMouseDeltas.end()) {
                totalValue += mouseIt->second * bind.scale;
            }
        }
    }

    return totalValue; 
}