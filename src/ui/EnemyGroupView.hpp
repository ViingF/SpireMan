#ifndef SPIRELIKE_ENEMYGROUPVIEW_HPP
#define SPIRELIKE_ENEMYGROUPVIEW_HPP
#include <SFML/Graphics.hpp>
#include "../model/Enemy.hpp"
#include "../core/ResourceManager.hpp"
#include <vector>

class EnemyGroupView {
public:
    void draw(
    sf::RenderWindow& window,
    const std::vector<Enemy>& enemies,
    const std::vector<sf::Texture*>& enemyTextures,
    const sf::Font& font
) const;


    int getEnemyIndexAtPosition(sf::Vector2f mousePos, int enemyCount) const;

private:
    sf::FloatRect getEnemyRect(int index, int total) const;
};


#endif //SPIRELIKE_ENEMYGROUPVIEW_HPP