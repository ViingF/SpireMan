
#ifndef SPIRELIKE_GAMECONTEXT_HPP
#define SPIRELIKE_GAMECONTEXT_HPP
#include "ResourceManager.hpp"
#include "database/CardDatabase.hpp"
#include "database/EncounterDatabase.hpp"
#include "database/EnemyDatabase.hpp"
#include "database/EventDatabase.hpp"
#include "model/RunState.hpp"

struct GameContext {
    ResourceManager& resources;
    RunState& runState;
    CardDatabase& cards;
    EnemyDatabase& enemies;
    EventDatabase& events;
    EncounterDatabase& encounters;

};

#endif //SPIRELIKE_GAMECONTEXT_HPP