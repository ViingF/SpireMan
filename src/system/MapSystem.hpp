#pragma once
#include "database/EncounterDatabase.hpp"
#include "database/EventDatabase.hpp"
#include "model/RunState.hpp"

class MapSystem {
public:
    void generateRouteMap(
        RunState& runState,
        const EventDatabase& eventDatabase
    ) const;

    void startAct(
        RunState& runState,
        int act,
        const EventDatabase& eventDatabase,
        const EncounterDatabase& encounterDatabase
    ) const;

    bool advanceToNextActIfPossible(
        RunState& runState,
        const EventDatabase& eventDatabase,
        const EncounterDatabase& encounterDatabase
    ) const;

    void refreshNodeStates(RunState& runState) const;

    ErrorCode selectNode(RunState& runState, int nodeIndex) const;

    void completeSelectedNode(RunState& runState) const;

    bool isRouteFinished(const RunState& runState) const;
};
