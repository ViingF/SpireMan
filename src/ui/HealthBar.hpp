// ui/HealthBar.hpp
#ifndef SPIRELIKE_HEALTHBAR_HPP
#define SPIRELIKE_HEALTHBAR_HPP

#include <SFML/Graphics.hpp>

class HealthBar {
public:
    static constexpr float DefaultWidth = 240.f;
    static constexpr float DefaultHeight = 30.f;

    explicit HealthBar(
        sf::Vector2f position,
        sf::Vector2f size = {DefaultWidth, DefaultHeight}
    );

    void draw(
        sf::RenderWindow& window,
        const sf::Texture* texture,
        const sf::Font& font,
        int hp,
        int maxHp
    ) const;

private:
    sf::Vector2f position_;
    sf::Vector2f size_;
};

#endif
