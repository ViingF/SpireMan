
#include "Button.hpp"

#include "TextUtils.hpp"


Button::Button(
    sf::Vector2f position,
    sf::Vector2f size,
    sf::Font& font,
    const std::string& text
) : content(font, TextUtils::fromUtf8(text))
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
    sf::Vector2f mousePos =
        window.mapPixelToCoords(sf::Mouse::getPosition(window));

    setHovered(shape.getGlobalBounds().contains(mousePos));

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
            sf::Vector2f clickPos =
                window.mapPixelToCoords(mousePressed->position);

            if (shape.getGlobalBounds().contains(clickPos)) {
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

void Button::setTexture(const sf::Texture& texture)
{
    shape.setTexture(&texture);

    normalFill_ = sf::Color::White;
    hoverFill_ = sf::Color(230, 230, 255);

    applyVisualState();
}


void Button::setText(const std::string& text)
{
    content.setString(TextUtils::fromUtf8(text));
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

    if (!enabled_) {
        hovered_ = false;
    }

    applyVisualState();
}


bool Button::isEnabled() const
{
    return enabled_;
}

void Button::setHovered(bool hovered)
{
    if (hovered_ == hovered) {
        return;
    }

    hovered_ = hovered;
    applyVisualState();
}

void Button::applyVisualState()
{
    if (!enabled_) {
        shape.setFillColor(sf::Color(70, 70, 70));
        shape.setOutlineThickness(0.f);
        content.setFillColor(sf::Color(150, 150, 150));
        return;
    }

    if (hovered_) {
        shape.setFillColor(hoverFill_);
        shape.setOutlineThickness(3.f);
        shape.setOutlineColor(sf::Color(255, 220, 120));
        content.setFillColor(sf::Color(255, 240, 180));
    } else {
        shape.setFillColor(normalFill_);
        shape.setOutlineThickness(0.f);
        content.setFillColor(sf::Color::White);
    }
}

void Button::update(const sf::Vector2i& mousePos)
{
    if (!enabled_) {
        hovered_ = false;
        applyVisualState();
        return;
    }

    sf::Vector2f pos(
        static_cast<float>(mousePos.x),
        static_cast<float>(mousePos.y)
    );

    setHovered(shape.getGlobalBounds().contains(pos));
}

void Button::setPlaceholderStyle()
{
    usesPlaceholder_ = true;
    shape.setTexture(nullptr);
    shape.setFillColor(sf::Color::White);
    shape.setOutlineColor(sf::Color(40, 40, 40));
    shape.setOutlineThickness(2.f);
    content.setFillColor(sf::Color(40, 40, 40));
}

void Button::setPosition(sf::Vector2f position)
{
    shape.setPosition(position);
    centerText();
}

void Button::setSize(sf::Vector2f size)
{
    shape.setSize(size);
    centerText();
}