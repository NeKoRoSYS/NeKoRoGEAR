#pragma once
#include "Core/Events/EventBus.h"
#include "InputCodes.h"
#include <unordered_map>
#include <vector>

struct KeyAxisBinding { KeyCode key; float scale; };
struct MouseAxisBinding { MouseAxis axis; float scale; };

class InputManager {
public:
    static InputManager& Get() {
        static InputManager instance;
        return instance;
    }

    void BindAction(InputAction action, KeyCode key);
    void BindAction(InputAction action, MouseButton button);
    void BindAxis(InputAxis axis, KeyCode key, float scale);
    void BindAxis(InputAxis axis, MouseAxis rawAxis, float scale = 1.0f);

    void Update();
    void ProcessRawEvent(const EngineEvent& event, EventBus& outEventBus);

    bool IsActionActive(InputAction action) const;
    bool WasActionJustPressed(InputAction action) const;
    float GetAxisValue(InputAxis axis) const;

private:
    InputManager() = default;

    std::unordered_map<KeyCode, std::vector<InputAction>> keyActionBindings;
    std::unordered_map<MouseButton, std::vector<InputAction>> mouseActionBindings;
    std::unordered_map<InputAction, InputState> actionStates;

    std::unordered_map<InputAxis, std::vector<KeyAxisBinding>> keyAxisBindings;
    std::unordered_map<InputAxis, std::vector<MouseAxisBinding>> mouseAxisBindings;
    std::unordered_map<MouseAxis, float> currentMouseDeltas;

    template<typename HardwareID>
    void ProcessHardwareState(HardwareID id, bool isPressed, EventBus& bus);

    const std::vector<InputAction>& GetBindings(KeyCode key) { return keyActionBindings[key]; }
    const std::vector<InputAction>& GetBindings(MouseButton btn) { return mouseActionBindings[btn]; }
};