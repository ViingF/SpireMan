
#include "Button.hpp"


Button::Button(
    sf::Vector2f position,
    sf::Vector2f size,
    const std::string& text
)
{
    shape.setPosition(position);
    shape.setSize(size);

    shape.setFillColor(
        sf::Color(80, 80, 120)
    );
}

void Button::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    if (
        const auto* mousePressed =
        event.getIf<sf::Event::MouseButtonPressed>()
    )
    {
        if (
            mousePressed->button ==
            sf::Mouse::Button::Left
        )
        {
            sf::Vector2f mousePos =
                window.mapPixelToCoords(
                    sf::Mouse::getPosition(window)
                );

            if (
                shape.getGlobalBounds()
                    .contains(mousePos)
            )
            {
                clicked = true;
            }
        }
    }
}

void Button::draw(
    sf::RenderWindow& window
) const
{
    window.draw(shape);
}

bool Button::wasClicked() const
{
    return clicked;
}

void Button::reset()
{
    clicked = false;
}
