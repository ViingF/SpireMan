
#ifndef SPIRELIKE_GAMECONTEXT_HPP
#define SPIRELIKE_GAMECONTEXT_HPP
#include "AudioManager.hpp"
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
    AudioManager& audio;
};

#endif //SPIRELIKE_GAMECONTEXT_HPP