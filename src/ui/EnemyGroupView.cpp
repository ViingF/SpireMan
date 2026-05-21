#include "EnemyGroupView.hpp"

#include "IntentView.hpp"
#include "StateView.hpp"
#include "TextUtils.hpp"

#include <algorithm>
#include <vector>

#include "HealthBar.hpp"

constexpr float kDesignWidth = 1920.f;
constexpr float kDesignHeight = 1080.f;

constexpr float kEnemySpriteWidth = 1500.f;
constexpr float kEnemySpriteHeight = 1000.f;
constexpr float kEnemyDisplayScale = 1.0f;
constexpr float kEnemyDisplayWidth = kEnemySpriteWidth * kEnemyDisplayScale;
constexpr float kEnemyDisplayHeight = kEnemySpriteHeight * kEnemyDisplayScale;
constexpr float kEnemyUILayoutDisplayWidth = kEnemySpriteWidth * 0.5f;

constexpr float kIntentSize = 64.f;
constexpr float kHealthBarWidth = 240.f;
constexpr float kHealthBarHeight = 30.f;
constexpr float kIntentGapAboveEnemy = 10.f;
constexpr float kIntentOffsetX = 380.f;
constexpr float kIntentOffsetY = 310.f;
constexpr float kEnemyHealthBarDesignY = 800.f;
constexpr float kStateGapBelowHealthBar = 19.f;

constexpr float kEnemyHitboxWidth = 260.f;
constexpr float kEnemyHitboxHeight = 360.f;
constexpr float kEnemyHitboxOffsetX =
    (kEnemyDisplayWidth - kEnemyHitboxWidth) * 0.5f;
constexpr float kEnemyHitboxOffsetY = 90.f;

struct EnemyLayoutConfig {
    std::vector<float> xPositions;
    float yPosition = 0.f;
};

void drawHealthBar(
    sf::RenderWindow& window,
    const sf::Texture& texture,
    const sf::Font& font,
    sf::Vector2f position,
    int hp,
    int maxHp
)
{
    if (maxHp <= 0) {
        return;
    }

    const int clampedHp = std::clamp(hp, 0, maxHp);
    const float ratio =
        static_cast<float>(clampedHp) / static_cast<float>(maxHp);

    sf::RectangleShape background({
        kHealthBarWidth,
        kHealthBarHeight
    });
    background.setFillColor(sf::Color(40, 40, 40, 180));
    background.setPosition(position);
    window.draw(background);

    sf::RectangleShape foreground({
        kHealthBarWidth * ratio,
        kHealthBarHeight
    });
    foreground.setTexture(&texture);
    foreground.setPosition(position);
    window.draw(foreground);

    sf::Text hpText = TextUtils::createWhiteText(
        font,
        TextUtils::formatHpText(clampedHp, maxHp),
        15,
        {position.x + 60.f, position.y + 5.f}
    );
    window.draw(hpText);
}

const EnemyLayoutConfig& getEnemyLayoutConfig(int enemyCount)
{
    static const EnemyLayoutConfig oneEnemy = {{690.f}, 217.f};
    static const EnemyLayoutConfig twoEnemies = {{690.f, 970.f}, 217.f};
    static const EnemyLayoutConfig threeEnemies = {
        {400.f, 690.f, 970.f},
        217.f
    };

    switch (enemyCount) {
        case 1: return oneEnemy;
        case 2: return twoEnemies;
        case 3: return threeEnemies;
        default: return threeEnemies;
    }
}

sf::FloatRect EnemyGroupView::getEnemyScreenRect(
    const sf::RenderWindow& window,
    int index,
    int total
) const
{
    const int slotCount = std::min(total, 3);

    if (slotCount <= 0 || index < 0 || index >= slotCount) {
        return sf::FloatRect({0.f, 0.f}, {0.f, 0.f});
    }

    const float scaleX =
        static_cast<float>(window.getSize().x) / kDesignWidth;
    const float scaleY =
        static_cast<float>(window.getSize().y) / kDesignHeight;

    const EnemyLayoutConfig& layout =
        getEnemyLayoutConfig(slotCount);

    return sf::FloatRect(
        {
            layout.xPositions[index] * scaleX,
            layout.yPosition * scaleY
        },
        {
            kEnemyDisplayWidth * scaleX,
            kEnemyDisplayHeight * scaleY
        }
    );
}

sf::FloatRect EnemyGroupView::getEnemyHitRect(
    const sf::RenderWindow& window,
    int index,
    int total
) const
{
    const int slotCount = std::min(total, 3);

    if (slotCount <= 0 || index < 0 || index >= slotCount) {
        return sf::FloatRect({0.f, 0.f}, {0.f, 0.f});
    }

    const float scaleX =
        static_cast<float>(window.getSize().x) / kDesignWidth;
    const float scaleY =
        static_cast<float>(window.getSize().y) / kDesignHeight;

    const EnemyLayoutConfig& layout =
        getEnemyLayoutConfig(slotCount);

    return sf::FloatRect(
        {
            (layout.xPositions[index] + kEnemyHitboxOffsetX) * scaleX,
            (layout.yPosition + kEnemyHitboxOffsetY) * scaleY
        },
        {
            kEnemyHitboxWidth * scaleX,
            kEnemyHitboxHeight * scaleY
        }
    );
}


const EnemyLayoutConfig& getLayoutConfig(int enemyCount)
{
    static const EnemyLayoutConfig oneEnemy = {{690.f}, 217.f};
    static const EnemyLayoutConfig twoEnemies = {{690.f, 970.f}, 217.f};
    static const EnemyLayoutConfig threeEnemies = {{400.f, 690.f, 970.f}, 217.f};

    switch (enemyCount) {
        case 1: return oneEnemy;
        case 2: return twoEnemies;
        case 3: return threeEnemies;
        default: return threeEnemies;
    }
}

void EnemyGroupView::draw(
    sf::RenderWindow& window,
    const std::vector<Enemy>& enemies,
    const std::vector<EnemyRenderData>& renderData,
    const sf::Font& font
) const
{
    const int slotCount = std::min(static_cast<int>(enemies.size()), 3);
    if (slotCount <= 0) {
        return;
    }

    const EnemyLayoutConfig& config = getLayoutConfig(slotCount);

    const float scaleX = static_cast<float>(window.getSize().x) / kDesignWidth;
    const float scaleY = static_cast<float>(window.getSize().y) / kDesignHeight;

    for (int enemyIndex = 0; enemyIndex < slotCount; ++enemyIndex) {
        const Enemy& enemy = enemies[enemyIndex];

        if (enemy.hp <= 0) {
            continue;
        }

        const EnemyRenderData emptyData;
        const EnemyRenderData& data =
            enemyIndex < static_cast<int>(renderData.size())
                ? renderData[enemyIndex]
                : emptyData;

        const float designEnemyX = config.xPositions[enemyIndex];
        const float designEnemyY = config.yPosition;

        const float designIntentX =
            designEnemyX +
            (kEnemyUILayoutDisplayWidth - kIntentSize) * 0.5f +
            kIntentOffsetX;

        const float designIntentY =
            designEnemyY -
            kIntentSize -
            kIntentGapAboveEnemy +
            kIntentOffsetY;

        const float designHealthX =
            designEnemyX + (kEnemyDisplayWidth - kHealthBarWidth) * 0.5f;

        const float designHealthY = kEnemyHealthBarDesignY;

        const float designStateY =
            designHealthY + kHealthBarHeight + kStateGapBelowHealthBar;

        IntentView intentView(
            {designIntentX * scaleX, designIntentY * scaleY},
            kIntentSize
        );
        intentView.draw(window, font, data.intent);

        const sf::FloatRect enemyScreenRect =
            getEnemyScreenRect(window, enemyIndex, slotCount);

        sf::RectangleShape enemyRect(enemyScreenRect.size);
        enemyRect.setPosition(enemyScreenRect.position);

        if (data.enemyTexture != nullptr) {
            enemyRect.setTexture(data.enemyTexture);
        } else {
            enemyRect.setFillColor(sf::Color::White);
        }

        enemyRect.setOutlineThickness(0.f);
        window.draw(enemyRect);

        HealthBar healthBar({
        designHealthX * scaleX,
        designHealthY * scaleY
    });

        healthBar.draw(
            window,
            data.hpTexture,
            font,
            enemy.hp,
            enemy.maxHp
        );


        StateView stateView({designHealthX * scaleX, designStateY * scaleY});
        stateView.draw(window, font, data.states);
    }
}

int EnemyGroupView::getEnemyIndexAtPosition(
    sf::Vector2f mousePos,
    const sf::RenderWindow& window,
    const std::vector<Enemy>& enemies
) const
{
    const int slotCount = std::min(static_cast<int>(enemies.size()), 3);

    for (int enemyIndex = slotCount - 1; enemyIndex >= 0; --enemyIndex) {
        if (enemies[enemyIndex].hp <= 0) {
            continue;
        }

        if (getEnemyHitRect(window, enemyIndex, slotCount).contains(mousePos)) {
            return enemyIndex;
        }
    }

    return -1;
}
