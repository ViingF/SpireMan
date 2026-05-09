
#include "Button.hpp"


Button::Button(
        sf::Vector2f position,
        sf::Vector2f size,
        sf::Font& font,
        const std::string& text):content(font,text) {
    shape.setPosition(position);
    sf::FloatRect bounds = content.getLocalBounds();
    content.setOrigin({
        bounds.position + bounds.size / 2.f
    });
    sf::Vector2f buttonCenter = {
        position.x + size.x / 2.f,
        position.y + size.y / 2.f
    };
    content.setPosition(buttonCenter);
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
    window.draw(content);
}

bool Button::wasClicked() const
{
    return clicked;
}

void Button::reset()
{
    clicked = false;
}

void Button::setTexture(const sf::Texture &texture) {
    shape.setFillColor(sf::Color::White);
    shape.setTexture(&texture);
}
