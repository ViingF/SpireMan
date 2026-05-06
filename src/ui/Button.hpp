
#ifndef SPIRELIKE_BUTTON_HPP
#define SPIRELIKE_BUTTON_HPP
#include <SFML/Graphics.hpp>

class Button {
public:
    Button(
        sf::Vector2f position,
        sf::Vector2f size,
        const std::string& text
    );

    void handleEvent(
        const sf::Event& event,
        const sf::RenderWindow& window
    );

    void draw(sf::RenderWindow& window) const;

    bool wasClicked() const;

    void reset();

private:
    sf::RectangleShape shape;
    bool clicked = false;
};
#endif //SPIRELIKE_BUTTON_HPP