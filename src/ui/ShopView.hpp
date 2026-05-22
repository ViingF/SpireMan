#ifndef SPIRELIKE_SHOPVIEW_HPP
#define SPIRELIKE_SHOPVIEW_HPP

#include <SFML/Graphics.hpp>
#include <vector>

#include "Button.hpp"
#include "CardView.hpp"
#include "../model/Types.hpp"
#include "core/GameContext.hpp"

struct CardDef;

struct ShopCardViewData {
    CardId cardId = 0;
    const CardDef* cardDef = nullptr;
    CardRenderTextures textures;
    int price = 0;
    bool sold = false;
};


struct ShopRemoveCardViewData {
    CardInstanceId instanceId = 0;
    CardId cardId = 0;
    const CardDef* cardDef = nullptr;
    CardRenderTextures textures;
};

class ShopView {
public:
    ShopView();

    void layoutButtons(
        Button& mapIconButton,
        Button& removeCardButton,
        Button& leaveButton,
        Button& cancelRemoveButton,
        Button& removePrevPageButton,
        Button& removeNextPageButton,
        const sf::RenderWindow& window
    ) const;

    bool handleCardClick(
        sf::Vector2f mousePos,
        int cardCount,
        int& clickedIndex
    ) const;

    bool handleRemoveCardClick(
        sf::Vector2f mousePos,
        int deckSize,
        int removePage,
        int& deckIndex
    ) const;

    int getRemoveCardsPerPage() const;

    void drawMain(
        sf::RenderWindow& window,
        GameContext& context,
        const sf::Font& font,
        const std::vector<ShopCardViewData>& cards,
        const std::string& message,
        int removeCost,
        Button& mapIconButton,
        Button& removeCardButton,
        Button& leaveButton
    ) const;

    void draw(
    sf::RenderWindow& window,
    const std::vector<ShopCardViewData>& cards,
    const sf::Font& font
) const;


private:
    sf::FloatRect getCardRect(int index) const;
    sf::FloatRect getRemoveCardRect(int visibleIndex) const;

private:
    float startX_ = 105.f;
    float startY_ = 245.f;
    float spacing_ = 56.f;
    float cardWidth_ = 294.f;
    float cardHeight_ = 414.f;
    int cardsPerRow_ = 3;

    float panelX_ = 1140.f;
    float panelY_ = 140.f;
    float panelWidth_ = 660.f;
    float panelHeight_ = 800.f;

    float removeCardStartX_ = 450.f;
    float removeCardStartY_ = 245.f;
    float removeCardWidth_ = 220.f;
    float removeCardHeight_ = 310.f;
    float removeCardGapX_ = 45.f;
    float removeCardGapY_ = 40.f;
    int removeColumns_ = 4;
    int removeRows_ = 2;
};


#endif // SPIRELIKE_SHOPVIEW_HPP
