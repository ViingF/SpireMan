
#ifndef SPIRELIKE_ENEMYDATABASE_HPP
#define SPIRELIKE_ENEMYDATABASE_HPP
#include <random>
#include <unordered_map>

#include "model/Enemy.hpp"
#include "model/Types.hpp"

class EnemyDatabase {
public:
    ErrorCode loadFromCsv(const std::string& path);

    const EnemyDef& get(EnemyId id) const;
    bool exists(EnemyId id) const;

    EnemyId chooseEnemyIdByFloor(
        int floor,
        std::mt19937& rng
    ) const;
    std::vector<EnemyId> getAllBossIds() const;
    EnemyId chooseRandomBossId(std::mt19937& rng) const;
    EnemyId chooseEnemyIdByAct(int act,std::mt19937& rng) const;

    EnemyId chooseRandomBossIdByAct(int act,std::mt19937& rng) const;


private:
    std::unordered_map<EnemyId, EnemyDef> enemies;
};

#endif //SPIRELIKE_ENEMYDATABASE_HPP