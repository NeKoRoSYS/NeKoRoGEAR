#pragma once
#include "Events.h"
#include <vector>

class EventBus {
public:
    void Push(const EngineEvent& event) {
        frameEvents.push_back(event);
    }

    const std::vector<EngineEvent>& GetEvents() const {
        return frameEvents;
    }

    void Clear() {
        frameEvents.clear();
    }

private:
    std::vector<EngineEvent> frameEvents;
};