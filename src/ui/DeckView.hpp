#ifndef SPIRELIKE_DECKVIEW_HPP
#define SPIRELIKE_DECKVIEW_HPP

#include <SFML/Graphics.hpp>
#include <vector>

#include "CardView.hpp"
#include "../model/Types.hpp"

struct CardDef;

struct DeckCardViewData {
    CardId cardId = 0;
    const CardDef* cardDef = nullptr;
    CardRenderTextures textures;
    bool selected = false;
};

class DeckView {
public:
    void setPage(int page);

    int getClickedIndex(
        sf::Vector2f mousePos,
        int cardCount
    ) const;

    void draw(
        sf::RenderWindow& window,
        const std::vector<DeckCardViewData>& cards,
        const sf::Font& font
    ) const;

private:
    sf::FloatRect getCardRect(int visibleIndex) const;

private:
    int page_ = 0;

    static constexpr int CARDS_PER_PAGE = 5;
};

#endif
