#include "InputManager.h"
#include <SDL3/SDL.h>

static KeyCode MapSDLToEngineKey(SDL_Keycode key) {
    switch (key) {
        case SDLK_W: return KeyCode::W;
        case SDLK_A: return KeyCode::A;
        case SDLK_S: return KeyCode::S;
        case SDLK_D: return KeyCode::D;
        case SDLK_Q: return KeyCode::Q;
        case SDLK_E: return KeyCode::E;
        case SDLK_SPACE: return KeyCode::Space;
        case SDLK_LSHIFT: return KeyCode::Shift;
        default: return KeyCode::Unknown;
    }
}

static SDL_Scancode MapEngineKeyToSDL(KeyCode key) {
    switch (key) {
        case KeyCode::W: return SDL_SCANCODE_W;
        case KeyCode::A: return SDL_SCANCODE_A;
        case KeyCode::S: return SDL_SCANCODE_S;
        case KeyCode::D: return SDL_SCANCODE_D;
        case KeyCode::Q: return SDL_SCANCODE_Q;
        case KeyCode::E: return SDL_SCANCODE_E;
        case KeyCode::Space: return SDL_SCANCODE_SPACE;
        case KeyCode::Shift: return SDL_SCANCODE_LSHIFT;
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
                ProcessHardwareState(MapSDLToEngineKey(e.key), true, outEventBus); 
            }
        } 
        else if constexpr (std::is_same_v<T, KeyPressEvent>) {
            if (!e.repeat) {
                ProcessHardwareState(MapSDLToEngineKey(e.key), e.pressed, outEventBus); 
            }
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

    if (auto it = mouseAxisBindings.find(axis); it != mouseAxisBindings.end()) {
        for (const auto& bind : it->second) {
            if (auto mouseIt = currentMouseDeltas.find(bind.axis); mouseIt != currentMouseDeltas.end()) {
                totalValue += mouseIt->second * bind.scale;
            }
        }
    }

    if (totalValue > 1.0f) return 1.0f;
    if (totalValue < -1.0f) return -1.0f;
    return totalValue;
}