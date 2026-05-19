#ifndef SPIRELIKE_BUTTON_HPP
#define SPIRELIKE_BUTTON_HPP

#include <SFML/Graphics.hpp>
#include <string>

class Button {
public:
    Button(
        sf::Vector2f position,
        sf::Vector2f size,
        sf::Font& font,
        const std::string& text
    );

    void handleEvent(
        const sf::Event& event,
        const sf::RenderWindow& window
    );

    void draw(sf::RenderWindow& window) const;

    void update(const sf::Vector2i& mousePos);

    void setTexture(const sf::Texture& texture);

    bool wasClicked() const;

    void reset();

    void setText(const std::string& text);

    void setEnabled(bool enabled);

    bool isEnabled() const;
    void setPlaceholderStyle();
private:
    void centerText();
    void applyVisualState();
    void setHovered(bool hovered);

    sf::RectangleShape shape;
    sf::Text content;
    sf::Vector2f buttonsize;

    bool clicked = false;
    bool enabled_ = true;
    bool hovered_ = false;
    bool usesPlaceholder_ = false;
    sf::Color normalFill_ = sf::Color(80, 80, 120);
    sf::Color hoverFill_ = sf::Color(110, 110, 170);
};

#endif
