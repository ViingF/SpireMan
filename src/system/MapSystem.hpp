#ifndef SPIRELIKE_MAPSYSTEM_HPP
#define SPIRELIKE_MAPSYSTEM_HPP

#include "database/EventDatabase.hpp"
#include "model/RunState.hpp"

class MapSystem {
public:
    void generateRouteMap(
        RunState& runState,
        const EventDatabase& eventDatabase
    ) const;

    void refreshNodeStates(RunState& runState) const;

    ErrorCode selectNode(RunState& runState, int nodeIndex) const;

    void completeSelectedNode(RunState& runState) const;

    bool isRouteFinished(const RunState& runState) const;
};

#endif // SPIRELIKE_MAPSYSTEM_HPP
