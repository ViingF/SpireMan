
#include "Button.hpp"


Button::Button(
    sf::Vector2f position,
    sf::Vector2f size,
    sf::Font& font,
    const std::string& text
) : content(font, text)
{
    shape.setPosition(position);
    shape.setSize(size);
    shape.setFillColor(sf::Color(80, 80, 120));
    content.setFillColor(sf::Color::White);

    centerText();
}



void Button::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    if (!enabled_) {
        clicked = false;
        return;
    }

    if (
        const auto* mousePressed =
        event.getIf<sf::Event::MouseButtonPressed>()
    )
    {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos =
                window.mapPixelToCoords(mousePressed->position);

            if (shape.getGlobalBounds().contains(mousePos)) {
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

void Button::setText(const std::string& text)
{
    content.setString(text);
    centerText();
}

void Button::centerText()
{
    sf::FloatRect bounds = content.getLocalBounds();

    content.setOrigin({
        bounds.position + bounds.size / 2.f
    });

    const sf::Vector2f position = shape.getPosition();
    const sf::Vector2f size = shape.getSize();

    content.setPosition({
        position.x + size.x / 2.f,
        position.y + size.y / 2.f
    });
}

void Button::setEnabled(bool enabled)
{
    enabled_ = enabled;
    clicked = false;

    if (enabled_) {
        shape.setFillColor(sf::Color(80, 80, 120));
        content.setFillColor(sf::Color::White);
    } else {
        shape.setFillColor(sf::Color(70, 70, 70));
        content.setFillColor(sf::Color(150, 150, 150));
    }
}

bool Button::isEnabled() const
{
    return enabled_;
}
