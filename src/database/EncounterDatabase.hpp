#ifndef SPIRELIKE_ENCOUNTERDATABASE_HPP
#define SPIRELIKE_ENCOUNTERDATABASE_HPP

#include "model/EncounterDef.hpp"
#include <random>
#include <unordered_map>
#include <vector>

class EncounterDatabase {
public:
    ErrorCode loadFromCsv(const std::string& path);

    bool exists(EncounterId id) const;
    const EncounterDef& get(EncounterId id) const;

    std::vector<EncounterId> getAllBossEncounterIds() const;

    EncounterId chooseEncounterIdByAct(
    int act,
    std::mt19937& rng
) const;

    EncounterId chooseRandomBossEncounterId(
        int act,
        std::mt19937& rng
    ) const;


private:
    std::unordered_map<EncounterId, EncounterDef> encounters_;
};

#endif
