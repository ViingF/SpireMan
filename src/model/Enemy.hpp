
#ifndef SPIRELIKE_ENEMY_HPP
#define SPIRELIKE_ENEMY_HPP
#include "Types.hpp"

struct EnemyIntent {
    EnemyIntentType type;
    int value;
};

struct EnemyDef {
    EnemyId id;
    std::string name;
    int maxHp;
    std::vector<EnemyIntent> movePattern;
};
struct Enemy {
    EnemyId id;
    std::string name;

    int maxHp;
    int hp;
    int block;

    int strength = 0;
    int vulnerable = 0;
    int weak = 0;

    EnemyIntent intent;
    std::vector<EnemyIntent> movePattern;
};
#endif //SPIRELIKE_ENEMY_HPP