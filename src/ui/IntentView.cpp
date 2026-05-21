#include "IntentView.hpp"

#include "TextUtils.hpp"

#include <string>

IntentView::IntentView(
    sf::Vector2f position,
    float size
)
    : position_(position),
      size_(size)
{
}

void IntentView::draw(
    sf::RenderWindow& window,
    const sf::Font& font,
    const IntentViewData& data
) const
{
    sf::RectangleShape icon({size_, size_});

    if (data.texture != nullptr) {
        icon.setTexture(data.texture);
        icon.setFillColor(sf::Color::White);
    } else {
        icon.setFillColor(sf::Color(255, 255, 255, 180));
    }

    icon.setPosition(position_);
    window.draw(icon);

    if (data.value < 0) {
        return;
    }

    sf::Text text = TextUtils::createWhiteText(
        font,
        std::to_string(data.value),
        25,
        {
            position_.x - 10.f,
            position_.y + size_ + 5.f
        }
    );

    window.draw(text);
}
