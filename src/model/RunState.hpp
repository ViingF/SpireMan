
#ifndef SPIRELIKE_RUNSTATE_HPP
#define SPIRELIKE_RUNSTATE_HPP
#include "Types.hpp"
#include "CardInstance.hpp"
#include "MapNode.hpp"
#include "EventDef.hpp"

#include <random>
#include <unordered_map>
#include <vector>

struct RunState {
    RunPlayerState player {70, 70};
    std::vector<CardInstance> masterDeck;

    int act = 1;
    int floor = 0;
    int floorInAct = 0;

    int gold = 0;

    std::vector<RelicId> relics;

    std::mt19937 rng;

    EnemyId currentEnemyId = 0;
    CardInstanceId nextCardInstanceId = 1;

    bool active = false;

    std::vector<MapNode> mapNodes;
    int currentMapNodeIndex = -1;
    int selectedMapNodeIndex = -1;
    EnemyId bossEnemyId = 0;
    int pendingRemoveCardCount = 0;
    std::vector<EventEffect> pendingEventEffects;
    std::unordered_map<EventId, int> eventDrawCounts;

};


#endif //SPIRELIKE_RUNSTATE_HPP