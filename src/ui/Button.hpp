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
        const std::string& text,
        const std::string& description = ""
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
    void setPosition(sf::Vector2f position);
    void setSize(sf::Vector2f size);
    void setDescription(const std::string& description);
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

    void drawDescription(sf::RenderWindow& window) const;

    const sf::Font* font_ = nullptr;
    std::string description_;
};

#endif
