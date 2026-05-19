#include "EnergyView.hpp"
#include "TextUtils.hpp"
#include <string>
EnergyView::EnergyView() {
    position = sf::Vector2f(130,820);
}

void EnergyView::draw(sf::RenderWindow &window,sf::Texture &texture,sf::Font &font,int nowEnergy,int maxEnergy) {
    sf::RectangleShape energy({128,128});
    energy.setTexture(&texture);
    energy.setPosition(position);

    sf::Text energyText = TextUtils::createWhiteText(
        font,
        TextUtils::formatHpText(nowEnergy, maxEnergy),
        30,
        {150, 870}
    );

    window.draw(energy);
    window.draw(energyText);
}
