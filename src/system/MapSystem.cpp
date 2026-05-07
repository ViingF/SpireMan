
#include "MapSystem.hpp"

#include "config/Constants.hpp"

#include <random>

void MapSystem::generateSingleRoute(
    RunState& runState,
    const EventDatabase& eventDatabase
) const
{
    runState.mapNodes.clear();
    runState.currentMapNodeIndex = -1;
    runState.selectedMapNodeIndex = -1;

    std::uniform_int_distribution<int> percentDist(1, 100);

    for (int i = 0; i < MAP_ROUTE_LENGTH; ++i) {
        MapNode node;
        node.index = i;
        node.state = MapNodeState::Locked;

        const bool isFirstNode = i == 0;
        const bool isLastNode = i == MAP_ROUTE_LENGTH - 1;

        if (isFirstNode || isLastNode) {
            node.type = MapNodeType::Combat;
        } else {
            const int roll = percentDist(runState.rng);
            const bool makeEvent =
                roll <= MAP_EVENT_CHANCE_PERCENT &&
                !eventDatabase.getAllEventIds().empty();

            if (makeEvent) {
                node.type = MapNodeType::Event;
                node.eventId = eventDatabase.chooseRandomEventId(runState.rng);
            } else {
                node.type = MapNodeType::Combat;
            }
        }

        runState.mapNodes.push_back(node);
    }

    refreshNodeStates(runState);
}

void MapSystem::refreshNodeStates(RunState& runState) const
{
    for (MapNode& node : runState.mapNodes) {
        if (node.state != MapNodeState::Completed) {
            node.state = MapNodeState::Locked;
        }
    }

    const int nextIndex = runState.currentMapNodeIndex + 1;

    if (nextIndex >= 0 &&
        nextIndex < static_cast<int>(runState.mapNodes.size()) &&
        runState.mapNodes[nextIndex].state != MapNodeState::Completed) {
        runState.mapNodes[nextIndex].state = MapNodeState::Available;
    }
}

ErrorCode MapSystem::selectNode(RunState& runState, int nodeIndex) const
{
    refreshNodeStates(runState);

    if (nodeIndex < 0 ||
        nodeIndex >= static_cast<int>(runState.mapNodes.size())) {
        return ErrorCode::INVALID_SCENE_STATE;
    }

    if (runState.mapNodes[nodeIndex].state != MapNodeState::Available) {
        return ErrorCode::INVALID_SCENE_STATE;
    }

    runState.selectedMapNodeIndex = nodeIndex;
    return ErrorCode::OK;
}

void MapSystem::completeSelectedNode(RunState& runState) const
{
    const int index = runState.selectedMapNodeIndex;

    if (index < 0 ||
        index >= static_cast<int>(runState.mapNodes.size())) {
        return;
    }

    runState.mapNodes[index].state = MapNodeState::Completed;
    runState.currentMapNodeIndex = index;
    runState.selectedMapNodeIndex = -1;

    refreshNodeStates(runState);
}

bool MapSystem::isRouteFinished(const RunState& runState) const
{
    if (runState.mapNodes.empty()) {
        return false;
    }

    return runState.currentMapNodeIndex >=
        static_cast<int>(runState.mapNodes.size()) - 1;
}