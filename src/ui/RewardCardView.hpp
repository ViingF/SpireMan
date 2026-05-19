#ifndef SPIRELIKE_REWARDCARDVIEW_HPP
#define SPIRELIKE_REWARDCARDVIEW_HPP

#include <SFML/Graphics.hpp>

#include "CardView.hpp"
#include "../model/CardDef.hpp"

class RewardCardView {
public:
    RewardCardView();

    void draw(
    sf::RenderWindow& window,
    const std::vector<CardDef>& cardDefs,
    const std::vector<CardRenderTextures>& textures,
    const sf::Font& font
) const;


private:
    sf::Vector2f position_;
};

#endif
