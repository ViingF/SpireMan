#include "DeckView.hpp"

#include "CardView.hpp"
#include "../model/CardDef.hpp"

#include <algorithm>

namespace {
    constexpr float CARD_START_X = 330.f;
    constexpr float CARD_START_Y = 780.f;
    constexpr float CARD_WIDTH = 330.f;
    constexpr float CARD_HEIGHT = 320.f;
    constexpr float CARD_SPACING = 280.f;
}

void DeckView::setPage(int page) {
    page_ = std::max(0, page);
}

sf::FloatRect DeckView::getCardRect(int visibleIndex) const {
    return sf::FloatRect(
        sf::Vector2f(
            CARD_START_X + visibleIndex * CARD_SPACING,
            CARD_START_Y
        ),
        sf::Vector2f(CARD_WIDTH, CARD_HEIGHT)
    );
}

int DeckView::getClickedIndex(
    sf::Vector2f mousePos,
    int cardCount
) const {
    const int startIndex = page_ * CARDS_PER_PAGE;
    const int endIndex = std::min(
        startIndex + CARDS_PER_PAGE,
        cardCount
    );

    for (int i = startIndex; i < endIndex; ++i) {
        const int visibleIndex = i - startIndex;

        if (getCardRect(visibleIndex).contains(mousePos)) {
            return i;
        }
    }

    return -1;
}

void DeckView::draw(
    sf::RenderWindow& window,
    const std::vector<DeckCardViewData>& cards,
    const sf::Font& font
) const
{
    const int startIndex =
        page_ * CARDS_PER_PAGE;

    const int endIndex =
        std::min(
            startIndex + CARDS_PER_PAGE,
            static_cast<int>(cards.size())
        );

    for (int i = startIndex; i < endIndex; ++i) {
        const DeckCardViewData& data =
            cards[i];

        if (data.cardDef == nullptr) {
            continue;
        }

        const int visibleIndex =
            i - startIndex;

        const sf::FloatRect rect =
            getCardRect(visibleIndex);

        CardView cardView(
            data.cardId,
            rect.position,
            rect.size
        );

        cardView.setSelected(data.selected);

        cardView.draw(
            window,
            *data.cardDef,
            data.textures,
            font
        );
    }
}

