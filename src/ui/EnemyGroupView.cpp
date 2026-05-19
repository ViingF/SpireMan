//
// Created by 21343 on 2026/5/15.
//

#include "EnemyGroupView.hpp"
#include "../model/Enemy.hpp"
#include "EnemyView.hpp"
#include <SFML/Graphics.hpp>

namespace {
    constexpr float ENEMY_SIZE = 512.f;

    // 不同数量敌人的布局配置
    struct LayoutConfig {
        std::vector<float> xPositions;
        float yPosition;
    };

    const LayoutConfig& getLayoutConfig(int enemyCount) {
        static const LayoutConfig oneEnemy = {{1250.f}, 745.f};
        static const LayoutConfig twoEnemies = {{1050.f, 1500.f}, 745.f};
        static const LayoutConfig threeEnemies = {{865.f, 1230.f, 1595.f}, 745.f};

        switch (enemyCount) {
            case 1: return oneEnemy;
            case 2: return twoEnemies;
            case 3: return threeEnemies;
            default: return threeEnemies; // 默认返回3个敌人的配置
        }
    }
}

void EnemyGroupView::draw(
    sf::RenderWindow& window,
    const std::vector<Enemy>& enemies,
    const std::vector<sf::Texture*>& enemyTextures,
    const sf::Font& font
) const {
    int enemyCount = std::min(static_cast<int>(enemies.size()), 3);

    if (enemyCount <= 0) {
        return;
    }

    const auto& config = getLayoutConfig(enemyCount);

    for (int i = 0; i < enemyCount; ++i) {
        if (i >= static_cast<int>(enemyTextures.size()) || enemyTextures[i] == nullptr) {
            continue;
        }

        const auto& enemy = enemies[i];
        sf::Vector2f position(config.xPositions[i], config.yPosition);

        EnemyView enemyView(position);
        enemyView.draw(window, enemy, *enemyTextures[i],font);
    }
}


int EnemyGroupView::getEnemyIndexAtPosition(sf::Vector2f mousePos, int enemyCount) const {
    enemyCount = std::min(enemyCount, 3);

    for (int i = 0; i < enemyCount; ++i) {
        sf::FloatRect rect = getEnemyRect(i, enemyCount);
        if (rect.contains(mousePos)) {
            return i;
        }
    }
    return -1;
}

sf::FloatRect EnemyGroupView::getEnemyRect(int index, int total) const {
    if (index < 0 || index >= total || total <= 0 || total > 3) {
        return sf::FloatRect({0.f, 0.f}, {0.f, 0.f});
    }

    const auto& config = getLayoutConfig(total);
    return sf::FloatRect(
        {config.xPositions[index],
        config.yPosition},
        {ENEMY_SIZE,
        ENEMY_SIZE});
}