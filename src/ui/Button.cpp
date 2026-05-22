
#include "Button.hpp"

#include "TextUtils.hpp"


Button::Button(
    sf::Vector2f position,
    sf::Vector2f size,
    sf::Font& font,
    const std::string& text,
    const std::string& description
) : content(font, TextUtils::fromUtf8(text)),
    font_(&font),
    description_(description)

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
    drawDescription(window);

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

void Button::setDescription(const std::string& description)
{
    description_ = description;
}

void Button::drawDescription(sf::RenderWindow& window) const
{
    if (!hovered_ || description_.empty() || font_ == nullptr) {
        return;
    }

    constexpr float padding = 8.0f;
    constexpr float gap = 8.0f;
    constexpr float margin = 4.0f;

    sf::Text tip(*font_, TextUtils::fromUtf8(description_));
    tip.setCharacterSize(22);
    tip.setFillColor(sf::Color::White);

    const sf::FloatRect textBounds = tip.getLocalBounds();

    const sf::Vector2f tipSize(
        textBounds.size.x + padding * 2.0f,
        textBounds.size.y + padding * 2.0f
    );

    const sf::Vector2f buttonPos = shape.getPosition();
    const sf::Vector2f buttonSize = shape.getSize();

    sf::Vector2f tipPos(
        buttonPos.x + buttonSize.x / 2.0f - tipSize.x / 2.0f,
        buttonPos.y - tipSize.y - gap
    );

    const sf::Vector2u windowSize = window.getSize();

    float maxX = static_cast<float>(windowSize.x) - tipSize.x - margin;
    if (maxX < margin) {
        maxX = margin;
    }

    if (tipPos.x < margin) {
        tipPos.x = margin;
    } else if (tipPos.x > maxX) {
        tipPos.x = maxX;
    }

    if (tipPos.y < margin) {
        tipPos.y = buttonPos.y + buttonSize.y + gap;
    }

    sf::RectangleShape background(tipSize);
    background.setPosition(tipPos);
    background.setFillColor(sf::Color(20, 20, 30, 230));
    background.setOutlineColor(sf::Color(255, 220, 120));
    background.setOutlineThickness(1.0f);

    tip.setOrigin(textBounds.position);
    tip.setPosition({
        tipPos.x + padding,
        tipPos.y + padding
    });

    window.draw(background);
    window.draw(tip);
}
