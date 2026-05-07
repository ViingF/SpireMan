
#ifndef SPIRELIKE_EVENTSYSTEM_HPP
#define SPIRELIKE_EVENTSYSTEM_HPP

#include "model/EventDef.hpp"
#include "model/RunState.hpp"

#include <string>

struct EventResolveResult {
    ErrorCode error = ErrorCode::OK;
    bool startCombat = false;
    bool playerDead = false;
    std::string message;
};

class EventSystem {
public:
    EventResolveResult resolveChoice(
        RunState& runState,
        const EventDef& eventDef,
        int choiceIndex
    ) const;
};
#endif //SPIRELIKE_EVENTSYSTEM_HPP