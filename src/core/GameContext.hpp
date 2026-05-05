
#ifndef SPIRELIKE_GAMECONTEXT_HPP
#define SPIRELIKE_GAMECONTEXT_HPP
#include "ResourceManager.hpp"
#include "database/CardDatabase.hpp"
#include "database/EnemyDatabase.hpp"

struct GameContext {
    ResourceManager& resources;
    RunState& runState;
    CardDatabase& cards;
    EnemyDatabase& enemies;
};

#endif //SPIRELIKE_GAMECONTEXT_HPP