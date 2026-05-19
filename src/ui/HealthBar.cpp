#include "HealthBar.hpp"

#include "TextUtils.hpp"

HealthBar::HealthBar(sf::Vector2f position)
    : position_(position)
{
}

void HealthBar::draw(
    sf::RenderWindow& window,
    const sf::Texture& texture,
    const sf::Font& font,
    int nowHp,
    int maxHp
) const {
    if (maxHp <= 0) {
        return;
    }

    const float scale =
        static_cast<float>(nowHp) / static_cast<float>(maxHp);

    sf::RectangleShape healthBar({240.f * scale, 30.f});
    healthBar.setTexture(&texture);
    healthBar.setPosition(position_);

    sf::Text text = TextUtils::createWhiteText(
        font,
        TextUtils::formatHpText(nowHp, maxHp),
        15,
        {position_.x + 60.f, position_.y + 10.f}
    );

    window.draw(healthBar);
    window.draw(text);
}
