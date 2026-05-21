#ifndef SPIRELIKE_INTENTVIEW_HPP
#define SPIRELIKE_INTENTVIEW_HPP

#include <SFML/Graphics.hpp>

struct IntentViewData {
    const sf::Texture* texture = nullptr;
    int value = -1;
};

class IntentView {
public:
    IntentView(
        sf::Vector2f position,
        float size
    );

    void draw(
        sf::RenderWindow& window,
        const sf::Font& font,
        const IntentViewData& data
    ) const;

private:
    sf::Vector2f position_;
    float size_ = 64.f;
};

#endif
