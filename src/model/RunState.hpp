
#ifndef SPIRELIKE_RUNSTATE_HPP
#define SPIRELIKE_RUNSTATE_HPP
#include "CardInstance.hpp"
#include "Types.hpp"

struct RunState {
    RunPlayerState player {70, 70};
    std::vector<CardInstance> masterDeck;

    int floor = 0;
    int gold = 0;

    std::vector<RelicId> relics;

    std::mt19937 rng;

    EnemyId currentEnemyId = 0;
    CardInstanceId nextCardInstanceId = 1;

    bool active = false;
};
#endif //SPIRELIKE_RUNSTATE_HPP