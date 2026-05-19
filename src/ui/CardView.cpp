#include "CardView.hpp"

#include "TextUtils.hpp"

#include <algorithm>
#include <sstream>
#include <vector>

namespace {

std::vector<std::string> wrapText(
    const std::string& text,
    std::size_t maxCharsPerLine
)
{
    std::vector<std::string> lines;
    std::istringstream input(text);

    std::string word;
    std::string line;

    while (input >> word) {
        if (
            !line.empty() &&
            line.size() + word.size() + 1 > maxCharsPerLine
        ) {
            lines.push_back(line);
            line.clear();
        }

        if (!line.empty()) {
            line += " ";
        }

        line += word;
    }

    if (!line.empty()) {
        lines.push_back(line);
    }

    return lines;
}

}

CardView::CardView(
    CardId id,
    sf::Vector2f position,
    sf::Vector2f size
)
    : id_(id),
      position_(position),
      size_(size),
      isSelected_(false)
{
}

void CardView::setSelected(bool selected)
{
    isSelected_ = selected;
}

sf::Vector2f CardView::getDrawPosition() const
{
    if (isSelected_) {
        return {
            position_.x,
            position_.y - 40.0f
        };
    }

    return position_;
}

bool CardView::contains(sf::Vector2f mousePos) const
{
    return sf::FloatRect(
        getDrawPosition(),
        size_
    ).contains(mousePos);
}

std::string CardView::cardTypeToString(CardType type)
{
    switch (type) {
    case CardType::Attack:
        return "Attack";

    case CardType::Skill:
        return "Skill";

    case CardType::Curse:
        return "Curse";
    }

    return "Unknown";
}

std::string CardView::costToString(int cost)
{
    if (cost < 0) {
        return "-";
    }

    return std::to_string(cost);
}

void CardView::drawTextureStretched(
    sf::RenderWindow& window,
    const sf::Texture& texture,
    sf::FloatRect targetRect
) const {
    const sf::Vector2u textureSize =
        texture.getSize();

    if (textureSize.x == 0 || textureSize.y == 0) {
        return;
    }

    sf::Sprite sprite(texture);

    sprite.setPosition({targetRect.position.x-60,targetRect.position.y-20});
    sprite.setScale({
        targetRect.size.x*1.4f / static_cast<float>(textureSize.x),
        targetRect.size.y*1.1f / static_cast<float>(textureSize.y)
    });

    window.draw(sprite);
}

void CardView::drawTextureFitted(
    sf::RenderWindow& window,
    const sf::Texture& texture,
    sf::FloatRect targetRect
) const
{
    const sf::Vector2u textureSize =
        texture.getSize();

    if (textureSize.x == 0 || textureSize.y == 0) {
        return;
    }

    const float scaleX =
        targetRect.size.x / static_cast<float>(textureSize.x);

    const float scaleY =
        targetRect.size.y / static_cast<float>(textureSize.y);

    const float scale =
        std::min(scaleX, scaleY);

    const float drawWidth =
        static_cast<float>(textureSize.x) * scale;

    const float drawHeight =
        static_cast<float>(textureSize.y) * scale;

    sf::Sprite sprite(texture);

    sprite.setScale({scale, scale});
    sprite.setPosition({
        targetRect.position.x +
            (targetRect.size.x - drawWidth) * 0.5f,
        targetRect.position.y +
            (targetRect.size.y - drawHeight) * 0.5f
    });

    window.draw(sprite);
}

void CardView::drawWrappedText(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& text,
    unsigned int size,
    sf::Vector2f position,
    float lineSpacing,
    sf::Color color,
    std::size_t maxCharsPerLine
)
{
    const std::vector<std::string> lines =
        wrapText(text, maxCharsPerLine);

    for (std::size_t i = 0; i < lines.size(); ++i) {
        sf::Text lineText(font);
        lineText.setString(lines[i]);
        lineText.setCharacterSize(size);
        lineText.setFillColor(color);
        lineText.setPosition({
            position.x,
            position.y + static_cast<float>(i) * lineSpacing
        });

        window.draw(lineText);
    }
}

void CardView::draw(
    sf::RenderWindow& window,
    const CardDef& def,
    const CardRenderTextures& textures,
    const sf::Font& font
) const
{
    const sf::Vector2f drawPosition =
        getDrawPosition();

    const sf::FloatRect cardRect(
        drawPosition,
        size_
    );

    if (textures.templateTexture != nullptr) {
        drawTextureStretched(
            window,
            *textures.templateTexture,
            cardRect
        );
    } else {
        sf::RectangleShape fallback(size_);
        fallback.setPosition(drawPosition);
        fallback.setFillColor(sf::Color(70, 70, 90));
        fallback.setOutlineThickness(3.0f);
        fallback.setOutlineColor(sf::Color::Black);
        window.draw(fallback);
    }

    const sf::FloatRect artRect(
        {
            drawPosition.x + size_.x * 0.13f,
            drawPosition.y + size_.y * 0.21f
        },
        {
            size_.x * 0.74f,
            size_.y * 0.34f
        }
    );

    if (textures.artTexture != nullptr) {
        drawTextureFitted(
            window,
            *textures.artTexture,
            artRect
        );
    }

    sf::Text costText = TextUtils::createWhiteText(
        font,
        costToString(def.cost),
        30,
        {
            drawPosition.x + size_.x * 0.1f,
            drawPosition.y + size_.y * 0.05f
        }
    );

    sf::Text nameText = TextUtils::createBlackText(
        font,
        def.name,
        22,
        {
            drawPosition.x + size_.x * 0.4f,
            drawPosition.y + size_.y * 0.09f
        }
    );

    sf::Text typeText = TextUtils::createGrayText(
        font,
        cardTypeToString(def.type),
        20,
        {
            drawPosition.x + size_.x * 0.43f,
            drawPosition.y + size_.y * 0.532f
        }
    );
    typeText.setFillColor(sf::Color::White);

    window.draw(costText);
    window.draw(nameText);
    window.draw(typeText);

    drawWrappedText(
        window,
        font,
        def.description,
        18,
        {
            drawPosition.x + size_.x * 0.19f,
            drawPosition.y + size_.y * 0.65f
        },
        24.0f,
        sf::Color::Black,
        22
    );

    if (isSelected_) {
        sf::RectangleShape border(size_);
        border.setPosition(drawPosition);
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineThickness(8.0f);
        border.setOutlineColor(sf::Color(255, 215, 0));
        window.draw(border);
    }
}
