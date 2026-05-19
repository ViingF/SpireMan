#ifndef SPIRELIKE_HEALTHBAR_HPP
#define SPIRELIKE_HEALTHBAR_HPP

#include <SFML/Graphics.hpp>

class HealthBar {
public:
    explicit HealthBar(sf::Vector2f position);

    void draw(
        sf::RenderWindow& window,
        const sf::Texture& texture,
        const sf::Font& font,
        int nowHp,
        int maxHp
    ) const;

private:
    sf::Vector2f position_;
};

#endif
