#ifndef SPIRELIKE_EVENTSYSTEM_HPP
#define SPIRELIKE_EVENTSYSTEM_HPP

#include "model/EventDef.hpp"
#include "model/RunState.hpp"

#include <string>
#include <vector>

class CardDatabase;

struct EventResolveResult {
    ErrorCode error = ErrorCode::OK;

    bool startCombat = false;
    bool playerDead = false;
    bool requiresCardRemove = false;

    std::string message;
};

class EventSystem {
public:
    EventResolveResult resolveChoice(
        RunState& runState,
        const EventDef& eventDef,
        int choiceIndex,
        const CardDatabase& cardDatabase
    ) const;

    EventResolveResult resolvePendingEventEffects(
        RunState& runState,
        const CardDatabase& cardDatabase
    ) const;

    ErrorCode removeCardByInstanceId(
        RunState& runState,
        CardInstanceId instanceId
    ) const;

private:
    EventResolveResult resolveEffects(
        RunState& runState,
        const std::vector<EventEffect>& effects,
        const CardDatabase& cardDatabase
    ) const;
};

#endif //SPIRELIKE_EVENTSYSTEM_HPP
