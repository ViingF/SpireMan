
#ifndef SPIRELIKE_CARDDEF_HPP
#define SPIRELIKE_CARDDEF_HPP

#include "Types.hpp"
struct CardDef {
    CardId id;
    std::string name;
    CardType type;
    int cost;
    TargetType target;
    std::vector<CardEffect> effects;
    std::string description;
    std::string textureId;
    bool exhaust = false;
};
#endif //SPIRELIKE_CARDDEF_HPP