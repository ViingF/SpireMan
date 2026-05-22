#include "TopInfoBar.hpp"

#include "TextUtils.hpp"

#include <string>

namespace {

constexpr float kDesignWidth = 1920.0f;
constexpr float kBarHeight = 70.0f;
constexpr float kIconSize = 64.0f;

sf::Vector2f getViewTopLeft(const sf::RenderWindow& window)
{
    const sf::View& view = window.getView();
    const sf::Vector2f center = view.getCenter();
    const sf::Vector2f size = view.getSize();

    return {
        center.x - size.x * 0.5f,
        center.y - size.y * 0.5f
    };
}

float getScaleX(const sf::RenderWindow& window)
{
    return window.getView().getSize().x / kDesignWidth;
}

sf::Vector2f getIconPosition(
    const sf::RenderWindow& window,
    float designX
)
{
    const sf::Vector2f topLeft = getViewTopLeft(window);

    return {
        topLeft.x + designX * getScaleX(window),
        topLeft.y
    };
}

void layoutIconButton(
    Button& button,
    const sf::RenderWindow& window,
    float designX
)
{
    button.setPosition(getIconPosition(window, designX));
    button.setSize({kIconSize, kIconSize});
}

void drawIcon(
    sf::RenderWindow& window,
    GameContext& context,
    const std::string& textureId,
    float designX
)
{
    if (!context.resources.hasTexture(textureId)) {
        return;
    }

    sf::RectangleShape icon({kIconSize, kIconSize});
    icon.setTexture(&context.resources.getTexture(textureId));
    icon.setPosition(getIconPosition(window, designX));

    window.draw(icon);
}

void drawIconValue(
    sf::RenderWindow& window,
    GameContext& context,
    const sf::Font& font,
    const std::string& textureId,
    float designX,
    int value
)
{
    if (!context.resources.hasTexture(textureId)) {
        return;
    }

    const sf::Vector2f iconPosition =
        getIconPosition(window, designX);

    sf::RectangleShape icon({kIconSize, kIconSize});
    icon.setTexture(&context.resources.getTexture(textureId));
    icon.setPosition(iconPosition);
    window.draw(icon);

    sf::Text text = TextUtils::createWhiteText(
        font,
        std::to_string(value),
        28,
        {
            iconPosition.x + kIconSize + 6.0f,
            iconPosition.y + 18.0f
        }
    );

    window.draw(text);
}

} // namespace

void TopInfoBar::draw(
    sf::RenderWindow& window,
    GameContext& context,
    const sf::Font& font,
    std::optional<int> hpOverride,
    bool drawDeckIcon,
    bool drawSettingsIcon,
    bool drawMapIcon
)
{
    const sf::Vector2f topLeft = getViewTopLeft(window);
    const sf::Vector2f viewSize = window.getView().getSize();

    sf::RectangleShape topBar({viewSize.x, kBarHeight});
    topBar.setPosition(topLeft);
    topBar.setFillColor(sf::Color(128, 128, 128));

    window.draw(topBar);

    const std::string playerName =
        context.runState.playerName.empty()
            ? "Player"
            : context.runState.playerName;

    sf::Text nameText = TextUtils::createWhiteText(
        font,
        playerName,
        28,
        {
            topLeft.x + 20.0f,
            topLeft.y + 18.0f
        }
    );

    window.draw(nameText);

    const int hp = hpOverride.value_or(context.runState.player.hp);

    drawIconValue(
        window,
        context,
        font,
        "panelHeart",
        265.0f,
        hp
    );

    drawIconValue(
        window,
        context,
        font,
        "panelGoldBag",
        425.0f,
        context.runState.gold
    );

    drawIconValue(
        window,
        context,
        font,
        "floor",
        890.0f,
        context.runState.floorInAct
    );

    if (drawMapIcon) {
        drawIcon(window, context, "map", 1700.0f);
    }

    if (drawDeckIcon) {
        drawIcon(window, context, "deck", 1773.0f);
    }

    if (drawSettingsIcon) {
        drawIcon(window, context, "settings", 1846.0f);
    }
}

void TopInfoBar::layoutDeckButton(
    Button& button,
    const sf::RenderWindow& window
)
{
    button.setDescription("查看牌堆");
    layoutIconButton(button, window, 1773.0f);
}

void TopInfoBar::layoutSettingsButton(
    Button& button,
    const sf::RenderWindow& window
)
{
    button.setDescription("保存并回到主菜单");
    layoutIconButton(button, window, 1846.0f);
}

void TopInfoBar::layoutMapButton(
    Button& button,
    const sf::RenderWindow& window
)
{
    button.setDescription("切换地图");
    layoutIconButton(button, window, 1700.0f);
}
