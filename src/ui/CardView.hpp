#ifndef SPIRELIKE_CARDVIEW_HPP
#define SPIRELIKE_CARDVIEW_HPP

#include <SFML/Graphics.hpp>
#include <string>

#include "../model/CardDef.hpp"

struct CardRenderTextures {
    const sf::Texture* templateTexture = nullptr;
    const sf::Texture* artTexture = nullptr;
};

class CardView {
public:
    CardView(
        CardId id,
        sf::Vector2f position,
        sf::Vector2f size = {294.f, 414.f}
    );

    void setSelected(bool selected);

    bool contains(sf::Vector2f mousePos) const;

    void draw(
        sf::RenderWindow& window,
        const CardDef& def,
        const CardRenderTextures& textures,
        const sf::Font& font
    ) const;

    void setDrawScale(sf::Vector2f scale);


private:
    sf::Vector2f getDrawPosition() const;

    void drawTextureStretched(
        sf::RenderWindow& window,
        const sf::Texture& texture,
        sf::FloatRect targetRect
    ) const;

    void drawTextureFitted(
        sf::RenderWindow& window,
        const sf::Texture& texture,
        sf::FloatRect targetRect
    ) const;

    static std::string cardTypeToString(CardType type);
    static std::string costToString(int cost);

    static void drawWrappedText(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& text,
    unsigned int size,
    sf::Vector2f position,
    float lineSpacing,
    sf::Color color,
    float maxWidth,
    std::size_t maxLines
);


private:
    CardId id_;
    sf::Vector2f position_;
    sf::Vector2f size_;
    bool isSelected_ = false;
    sf::Vector2f drawScale_{1.f, 1.f};
};

#endif // SPIRELIKE_CARDVIEW_HPP
