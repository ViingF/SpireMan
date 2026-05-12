
#ifndef SPIRELIKE_CARDDATABASE_HPP
#define SPIRELIKE_CARDDATABASE_HPP

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <vector>

#include "model/CardDef.hpp"
#include "model/Types.hpp"

class CardDatabase {
public:
    ErrorCode loadFromCsv(const std::string& path);

    const CardDef& get(CardId id) const;
    bool exists(CardId id) const;

    std::vector<CardId> getAllCardIds() const;
    std::vector<CardId> getCardIdsByType(CardType type) const;
    std::vector<CardId> getRewardCardIds() const;

    CardId chooseRandomCardId(std::mt19937& rng) const;
    CardId chooseRandomRewardCardId(std::mt19937& rng) const;

private:
    std::unordered_map<CardId, CardDef> cards;
};


#endif //SPIRELIKE_CARDDATABASE_HPP