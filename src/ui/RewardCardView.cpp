#include "RewardCardView.hpp"
#include "CardView.hpp"

RewardCardView::RewardCardView()
    : position_(430.f, 400.f)
{
}

void RewardCardView::draw(
    sf::RenderWindow& window,
    const std::vector<CardDef>& cardDefs,
    const std::vector<CardRenderTextures>& textures,
    const sf::Font& font
) const
{
    const float spacing = 340.f;

    const std::size_t count =
        std::min(cardDefs.size(), textures.size());

    for (std::size_t i = 0; i < count; ++i) {
        sf::Vector2f cardPosition(
            position_.x + static_cast<float>(i) * spacing,
            position_.y
        );

        CardView cardView(
            cardDefs[i].id,
            cardPosition
        );

        cardView.draw(
            window,
            cardDefs[i],
            textures[i],
            font
        );
    }
}

