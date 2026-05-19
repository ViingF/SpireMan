#include "StateView.hpp"

#include "TextUtils.hpp"

StateView::StateView(sf::Vector2f position)
    : position_(position)
{
}

void StateView::draw(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::vector<StateIconViewData>& states
) const {
    float currentX = position_.x;
    const float spacing = 55.f;

    for (const StateIconViewData& state : states) {
        if (state.value <= 0 || state.texture == nullptr) {
            continue;
        }

        sf::RectangleShape icon({48.f, 48.f});
        icon.setPosition({currentX, position_.y});
        icon.setTexture(state.texture);

        sf::Text valueText = TextUtils::createWhiteText(
            font,
            std::to_string(state.value),
            15,
            {currentX - 20.f, position_.y + 40.f}
        );

        window.draw(icon);
        window.draw(valueText);

        currentX += spacing;
    }
}
