#include "EnergyView.hpp"
#include "TextUtils.hpp"
#include <string>
EnergyView::EnergyView() {
    position = sf::Vector2f(70, 760);
}

void EnergyView::draw(sf::RenderWindow &window,sf::Texture &texture,sf::Font &font,int nowEnergy,int maxEnergy) {
    sf::RectangleShape energy({256,256});
    energy.setTexture(&texture);
    energy.setPosition(position);

    sf::Text energyText = TextUtils::createBlackText(
        font,
        TextUtils::formatHpText(nowEnergy, maxEnergy),
        40,
        {170, 870}
    );

    window.draw(energy);
    window.draw(energyText);
}
