#include "CardView.hpp"

#include "TextUtils.hpp"

#include <algorithm>
#include <sstream>
#include <vector>

namespace {

    std::vector<sf::String> wrapTextByWidth(
    const std::string& text,
    const sf::Font& font,
    unsigned int size,
    float maxWidth
)
    {
        std::vector<sf::String> lines;

        sf::String source = sf::String::fromUtf8(
            text.begin(),
            text.end()
        );

        sf::String current;
        sf::Text probe(font);
        probe.setCharacterSize(size);

        for (auto ch : source) {
            if (ch == '\n') {
                lines.push_back(current);
                current.clear();
                continue;
            }

            sf::String candidate = current;
            candidate += ch;

            probe.setString(candidate);
            const sf::FloatRect bounds = probe.getLocalBounds();

            if (!current.isEmpty() && bounds.size.x > maxWidth) {
                lines.push_back(current);
                current.clear();
                current += ch;
            } else {
                current = candidate;
            }
        }

        if (!current.isEmpty()) {
            lines.push_back(current);
        }

        return lines;
    }


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

    void setTextCenterX(sf::Text& text, float centerX)
{
    const sf::FloatRect bounds = text.getLocalBounds();

    text.setOrigin({
        bounds.position.x + bounds.size.x * 0.5f,
        bounds.position.y
    });

    text.setPosition({
        centerX,
        text.getPosition().y
    });
}

    float cardTextScale(sf::Vector2f cardSize)
{
    constexpr float baseWidth = 294.f;
    constexpr float baseHeight = 414.f;

    return std::min(
        cardSize.x / baseWidth,
        cardSize.y / baseHeight
    );
}

    unsigned int scaledFontSize(unsigned int baseSize, float scale)
{
    return static_cast<unsigned int>(
        std::max(10.f, std::round(static_cast<float>(baseSize) * scale))
    );
}

    void centerText(sf::Text& text, sf::Vector2f center)
{
    const sf::FloatRect bounds = text.getLocalBounds();

    text.setOrigin({
        bounds.position.x + bounds.size.x * 0.5f,
        bounds.position.y + bounds.size.y * 0.5f
    });

    text.setPosition(center);
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
    const sf::Vector2u textureSize = texture.getSize();

    if (textureSize.x == 0 || textureSize.y == 0) {
        return;
    }

    constexpr float expandX = 1.4f;
    constexpr float expandY = 1.1f;

    const float drawWidth = targetRect.size.x * expandX;
    const float drawHeight = targetRect.size.y * expandY;

    sf::Sprite sprite(texture);

    sprite.setScale({
        drawWidth / static_cast<float>(textureSize.x),
        drawHeight / static_cast<float>(textureSize.y)
    });

    sprite.setPosition({
        targetRect.position.x - (drawWidth - targetRect.size.x) * 0.5f,
        targetRect.position.y - (drawHeight - targetRect.size.y) * 0.5f
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
    float maxWidth,
    std::size_t maxLines
)
{
    std::vector<sf::String> lines =
        wrapTextByWidth(text, font, size, maxWidth);

    if (lines.size() > maxLines) {
        lines.resize(maxLines);

        if (!lines.empty()) {
            lines.back() += "...";
        }
    }

    for (std::size_t i = 0; i < lines.size(); ++i) {
        sf::Text lineText(font);
        lineText.setString(lines[i]);
        lineText.setCharacterSize(size);
        lineText.setFillColor(color);

        centerText(lineText, {
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

    const float textScale = cardTextScale(size_);

    const unsigned int costFontSize = scaledFontSize(50, textScale);
    const unsigned int nameFontSize = scaledFontSize(25, textScale);
    const unsigned int typeFontSize = scaledFontSize(20, textScale);
    const unsigned int descFontSize = scaledFontSize(24, textScale);

    const float descLineSpacing = 24.f * textScale;

    const float descMaxWidth = size_.x * 0.68f;


    const std::size_t descMaxLines =
        size_.y < 330.f ? 3 : 4;


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
    costFontSize,
    {0.f, 0.f}
);
    costText.setFillColor(sf::Color::White);
    costText.setStyle(sf::Text::Bold);
    costText.setOutlineColor(sf::Color::Yellow);
    costText.setOutlineThickness(1.0f);

    centerText(costText, {
        drawPosition.x + size_.x * 0.125f,
        drawPosition.y + size_.y * 0.095f
    });


    sf::Text nameText = TextUtils::createBlackText(
    font,
    def.name,
    nameFontSize,
    {0.f, 0.f}
);

    centerText(nameText, {
        drawPosition.x + size_.x * 0.5f,
        drawPosition.y + size_.y * 0.115f
    });

    setTextCenterX(nameText, drawPosition.x + size_.x * 0.5f);


    sf::Text typeText = TextUtils::createGrayText(
    font,
    cardTypeToString(def.type),
    typeFontSize,
    {0.f, 0.f}
);

    typeText.setFillColor(sf::Color::White);

    centerText(typeText, {
        drawPosition.x + size_.x * 0.5f,
        drawPosition.y + size_.y * 0.56f
    });

    setTextCenterX(typeText, drawPosition.x + size_.x * 0.5f);

    typeText.setFillColor(sf::Color::White);

    window.draw(costText);
    window.draw(nameText);
    window.draw(typeText);

    drawWrappedText(
    window,
    font,
    def.description,
    descFontSize,
    {
        drawPosition.x + size_.x * 0.5f,
        drawPosition.y + size_.y * 0.675f
    },
    descLineSpacing,
    sf::Color::Black,
    descMaxWidth,
    descMaxLines
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

void CardView::setDrawScale(sf::Vector2f scale)
{
    drawScale_ = scale;
}