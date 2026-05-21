#ifndef SPIRELIKE_ENEMYGROUPVIEW_HPP
#define SPIRELIKE_ENEMYGROUPVIEW_HPP

#include <SFML/Graphics.hpp>
#include "../model/Enemy.hpp"
#include "IntentView.hpp"
#include "StateView.hpp"
#include <vector>

struct EnemyRenderData {
    const sf::Texture* enemyTexture = nullptr;
    const sf::Texture* hpTexture = nullptr;
    IntentViewData intent;
    std::vector<StateIconViewData> states;
};

class EnemyGroupView {
public:
    void draw(
        sf::RenderWindow& window,
        const std::vector<Enemy>& enemies,
        const std::vector<EnemyRenderData>& renderData,
        const sf::Font& font
    ) const;

    int getEnemyIndexAtPosition(
        sf::Vector2f mousePos,
        const sf::RenderWindow& window,
        const std::vector<Enemy>& enemies
    ) const;

private:
    sf::FloatRect getEnemyScreenRect(
        const sf::RenderWindow& window,
        int index,
        int total
    ) const;

    sf::FloatRect getEnemyHitRect(
        const sf::RenderWindow& window,
        int index,
        int total
    ) const;
};

#endif
