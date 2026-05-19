#ifndef SPIRELIKE_SHOPVIEW_HPP
#define SPIRELIKE_SHOPVIEW_HPP

#include <SFML/Graphics.hpp>
#include <vector>

#include "CardView.hpp"
#include "../model/Types.hpp"

struct CardDef;

struct ShopCardViewData {
    CardId cardId = 0;
    const CardDef* cardDef = nullptr;
    CardRenderTextures textures;
    bool sold = false;
};


class ShopView {
public:
    ShopView();

    bool handleCardClick(
        sf::Vector2f mousePos,
        int cardCount,
        int& clickedIndex
    ) const;

    void draw(
        sf::RenderWindow& window,
        const std::vector<ShopCardViewData>& cards,
        const sf::Font& font
    ) const;

private:
    sf::FloatRect getCardRect(int index) const;

private:
    float startX_ = 215.f;
    float startY_ = 185.f;
    float spacing_ = 60.f;
    float cardWidth_ = 390.f;
    float cardHeight_ = 400.f;

    int cardsPerRow_ = 3;
};

#endif // SPIRELIKE_SHOPVIEW_HPP
