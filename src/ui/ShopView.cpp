#include "../ui/ShopView.hpp"

#include "../ui/CardView.hpp"
#include "../ui/TextUtils.hpp"
#include "../model/CardDef.hpp"

ShopView::ShopView() = default;

bool ShopView::handleCardClick(
    sf::Vector2f mousePos,
    int cardCount,
    int& clickedIndex
) const {
    for (int i = 0; i < cardCount; ++i) {
        if (getCardRect(i).contains(mousePos)) {
            clickedIndex = i;
            return true;
        }
    }

    return false;
}

sf::FloatRect ShopView::getCardRect(int index) const {
    const int row = index / cardsPerRow_;
    const int col = index % cardsPerRow_;

    const float x = startX_ + col * (cardWidth_ + spacing_);
    const float y = startY_ + row * (cardHeight_ + spacing_ + 60.f);

    return sf::FloatRect(
        sf::Vector2f(x, y),
        sf::Vector2f(cardWidth_, cardHeight_)
    );
}

void ShopView::draw(
    sf::RenderWindow& window,
    const std::vector<ShopCardViewData>& cards,
    const sf::Font& font
) const
{
    for (int i = 0; i < static_cast<int>(cards.size()); ++i) {
        const ShopCardViewData& data =
            cards[i];

        if (data.cardDef == nullptr) {
            continue;
        }

        const sf::FloatRect rect =
            getCardRect(i);

        CardView cardView(
            data.cardId,
            rect.position,
            rect.size
        );

        cardView.draw(
            window,
            *data.cardDef,
            data.textures,
            font
        );

        if (data.sold) {
            sf::RectangleShape overlay;
            overlay.setSize(rect.size);
            overlay.setPosition(rect.position);
            overlay.setFillColor(sf::Color(0, 0, 0, 150));
            window.draw(overlay);

            sf::Text soldText = TextUtils::createWhiteText(
                font,
                "已售出",
                24,
                {
                    rect.position.x + rect.size.x * 0.5f - 40.0f,
                    rect.position.y + rect.size.y * 0.5f - 12.0f
                }
            );

            window.draw(soldText);
        }
    }
}

