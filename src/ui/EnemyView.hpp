//
// Created by 21343 on 2026/5/8.
//

#ifndef SPIRELIKE_ENEMYVIEW_HPP
#define SPIRELIKE_ENEMYVIEW_HPP
#include <SFML/Graphics.hpp>
#include "../model/Enemy.hpp"

class EnemyView {
public:
    EnemyView(sf::Vector2f position);
    void draw(sf::RenderWindow& window, const Enemy& enemy, sf::Texture &texture, const sf::Font &font) const;
private:
    sf::Vector2f position;
};


#endif //SPIRELIKE_ENEMYVIEW_HPP