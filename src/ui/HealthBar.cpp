// ui/HealthBar.cpp
#include "HealthBar.hpp"

#include "TextUtils.hpp"

#include <algorithm>

HealthBar::HealthBar(
    sf::Vector2f position,
    sf::Vector2f size
)
    : position_(position),
      size_(size)
{
}

void HealthBar::draw(
    sf::RenderWindow& window,
    const sf::Texture* texture,
    const sf::Font& font,
    int hp,
    int maxHp
) const {
    if (maxHp <= 0) {
        return;
    }

    const int clampedHp = std::clamp(hp, 0, maxHp);
    const float ratio =
        static_cast<float>(clampedHp) / static_cast<float>(maxHp);

    sf::RectangleShape background(size_);
    background.setFillColor(sf::Color(40, 40, 40, 180));
    background.setPosition(position_);
    window.draw(background);

    sf::RectangleShape foreground({
        size_.x * ratio,
        size_.y
    });

    if (texture != nullptr) {
        foreground.setTexture(texture);
    } else {
        foreground.setFillColor(sf::Color(180, 40, 40));
    }

    foreground.setPosition(position_);
    window.draw(foreground);

    sf::Text hpText = TextUtils::createWhiteText(
        font,
        TextUtils::formatHpText(clampedHp, maxHp),
        15,
        {position_.x + 60.f, position_.y + 5.f}
    );

    window.draw(hpText);
}
