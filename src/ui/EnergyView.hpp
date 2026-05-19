#ifndef SPIRELIKE_ENERGYVIEW_HPP
#define SPIRELIKE_ENERGYVIEW_HPP
#include <SFML/Graphics.hpp>
#include <iostream>

class EnergyView {
private:
    sf::Vector2f position;
public:
    EnergyView();
    void draw(sf::RenderWindow& window,sf::Texture &texture,sf::Font &font,int nowEnergy,int maxEnergy);
};


#endif //SPIRELIKE_ENERGYVIEW_HPP