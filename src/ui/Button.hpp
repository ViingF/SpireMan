
#ifndef SPIRELIKE_BUTTON_HPP
#define SPIRELIKE_BUTTON_HPP
#include <SFML/Graphics.hpp>

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

private:
    sf::RectangleShape shape;
    sf::Text content;
    sf::Vector2f buttonsize;
    bool clicked = false;
};
#endif //SPIRELIKE_BUTTON_HPP