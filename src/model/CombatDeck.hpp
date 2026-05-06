
#ifndef SPIRELIKE_COMBATDECK_HPP
#define SPIRELIKE_COMBATDECK_HPP
#include "CardInstance.hpp"

struct CombatDeck {
    std::vector<CardInstance> drawPile;
    std::vector<CardInstance> hand;
    std::vector<CardInstance> discardPile;
    std::vector<CardInstance> exhaustPile;
};
#endif //SPIRELIKE_COMBATDECK_HPP