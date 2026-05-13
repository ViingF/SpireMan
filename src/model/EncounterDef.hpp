#ifndef SPIRELIKE_ENCOUNTERDEF_HPP
#define SPIRELIKE_ENCOUNTERDEF_HPP

#include "Types.hpp"
#include <string>
#include <vector>

struct EncounterEnemySlot {
    EnemyId enemyId = 0;
};

struct EncounterDef {
    EncounterId id = 0;
    std::string name;

    std::vector<EncounterEnemySlot> enemies;

    bool isBoss = false;
    int act = 1;
};


#endif
