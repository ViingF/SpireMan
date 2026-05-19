#ifndef SPIRELIKE_STATEVIEW_HPP
#define SPIRELIKE_STATEVIEW_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct StateIconViewData {
    int value = 0;
    const sf::Texture* texture = nullptr;
};

class StateView {
public:
    explicit StateView(sf::Vector2f position);

    void draw(
        sf::RenderWindow& window,
        const sf::Font& font,
        const std::vector<StateIconViewData>& states
    ) const;

private:
    sf::Vector2f position_;
};

#endif
