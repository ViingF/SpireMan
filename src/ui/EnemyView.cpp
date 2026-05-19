//
// Created by 21343 on 2026/5/8.
//

#include "EnemyView.hpp"
#include "TextUtils.hpp"

EnemyView::EnemyView(sf::Vector2f position) : position(position) {}

void EnemyView::draw(sf::RenderWindow& window, const Enemy& enemy, sf::Texture& texture, const sf::Font& font) const {
    sf::RectangleShape rectangle;
    rectangle.setTexture(&texture);
    rectangle.setPosition(position);


    window.draw(rectangle);

    sf::Text nameText = TextUtils::createWhiteText(font, enemy.name, 24, sf::Vector2f(position.x, position.y - 50));
    window.draw(nameText);
}
