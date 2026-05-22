#pragma once
#include "model/Enemy.hpp"
#include "model/Player.hpp"

class EnemyAISystem
{
public:
    EnemyIntent chooseIntent(const Enemy& enemy, int turnIndex) const;//生成意图
    void applyIntent(Enemy& enemy, Player& player) const;//执行意图
};
