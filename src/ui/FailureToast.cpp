#include "FailureToast.hpp"

#include "TextUtils.hpp"

#include <algorithm>

void FailureToast::show(const std::string& message)
{
    message_ = message.empty() ? "操作失败" : message;
    remainSeconds_ = kDurationSeconds;
}

void FailureToast::update(float dt)
{
    if (remainSeconds_ <= 0.0f) {
        return;
    }

    remainSeconds_ -= dt;

    if (remainSeconds_ <= 0.0f) {
        remainSeconds_ = 0.0f;
        message_.clear();
    }
}

void FailureToast::draw(sf::RenderWindow& window, const sf::Font& font) const
{
    if (remainSeconds_ <= 0.0f || message_.empty()) {
        return;
    }

    sf::Text text(font);
    text.setString(TextUtils::fromUtf8(message_));
    text.setCharacterSize(34);
    text.setFillColor(sf::Color::White);

    const sf::FloatRect textBounds = text.getLocalBounds();

    const float paddingX = 42.0f;
    const float paddingY = 24.0f;

    const float boxWidth = std::max(320.0f, textBounds.size.x + paddingX * 2.0f);
    const float boxHeight = std::max(86.0f, textBounds.size.y + paddingY * 2.0f);

    const sf::Vector2u winSize = window.getSize();
    const sf::Vector2f center = window.mapPixelToCoords({
        static_cast<int>(winSize.x / 2),
        static_cast<int>(winSize.y / 2)
    });

    sf::RectangleShape background;
    background.setSize({boxWidth, boxHeight});
    background.setOrigin({boxWidth / 2.0f, boxHeight / 2.0f});
    background.setPosition(center);
    background.setFillColor(sf::Color(40, 20, 20, 220));
    background.setOutlineThickness(3.0f);
    background.setOutlineColor(sf::Color(230, 80, 80, 240));

    text.setOrigin({
        textBounds.position.x + textBounds.size.x / 2.0f,
        textBounds.position.y + textBounds.size.y / 2.0f
    });
    text.setPosition(center);

    window.draw(background);
    window.draw(text);
}
