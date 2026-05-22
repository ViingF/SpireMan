
#ifndef SPIRELIKE_CONSTANTS_HPP
#define SPIRELIKE_CONSTANTS_HPP

constexpr int MAP_ROUTE_LENGTH = 9;
constexpr int MAP_EVENT_CHANCE_PERCENT = 30;
constexpr int MAP_CAMPFIRE_CHANCE_PERCENT = 20;
constexpr int CAMPFIRE_HEAL_PERCENT = 30;
constexpr int MAP_SHOP_CHANCE_PERCENT = 30;
constexpr int SHOP_CARD_COUNT = 3;
constexpr int SHOP_CARD_COST = 50;
constexpr int MAP_LAYER_COUNT = 8;

constexpr int MAP_MIN_NODES_PER_LAYER = 2;
constexpr int MAP_MAX_NODES_PER_LAYER = 5;

constexpr int MAP_MAX_EXTRA_EDGES_PER_NODE = 2;
constexpr int MAP_MAX_BRANCH_COUNT = 3;
constexpr int MAP_MAX_SHOP_COUNT = 2;
constexpr int MAP_MAX_EVENT_COUNT = 6;
constexpr int MAP_MAX_CAMPFIRE_COUNT = 7;


constexpr int MAX_ACT = 3;

constexpr int ACT1_ROUTE_LENGTH = 9;
constexpr int ACT2_ROUTE_LENGTH = 10;
constexpr int ACT3_ROUTE_LENGTH = 11;

constexpr int SHOP_REMOVE_BASE_COST = 50;


inline int getRouteLengthByAct(int act)
{
    if (act == 1) {
        return ACT1_ROUTE_LENGTH;
    }

    if (act == 2) {
        return ACT2_ROUTE_LENGTH;
    }

    return ACT3_ROUTE_LENGTH;
}

constexpr int ENEMY_HP_PERCENT = 30;
constexpr int ENEMY_ATTACK_PERCENT = 100;

// 额外固定加成
constexpr int ENEMY_HP_FLAT_BONUS = 0;
constexpr int ENEMY_ATTACK_FLAT_BONUS = 0;

inline int adjustEnemyValue(
    int baseValue,
    int percent,
    int flatBonus,
    int minValue
)
{
    int value = static_cast<int>(baseValue * percent / 100.0f + 0.5f);
    value += flatBonus;

    if (value < minValue) {
        return minValue;
    }

    return value;
}



#define WINDOWSOULUTION {1920,1080}


#endif //SPIRELIKE_CONSTANTS_HPP