#include "MapScene.hpp"

#include "config/Constants.hpp"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>

namespace {



sf::Text makeText(
    const sf::Font& font,
    const std::string& content,
    unsigned int size
)
{
    return sf::Text(font, content, size);
}

bool readLeftClickPosition(
    const sf::Event& event,
    sf::Vector2i& pixelPosition
)
{
    if (const auto* mouse =
            event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouse->button == sf::Mouse::Button::Left) {
            pixelPosition = mouse->position;
            return true;
        }
    }

    return false;
}

sf::Vector2f getViewTopLeft(const sf::RenderWindow& window)
{
    const sf::View& view = window.getView();
    const sf::Vector2f center = view.getCenter();
    const sf::Vector2f size = view.getSize();

    return sf::Vector2f(
        center.x - size.x * 0.5f,
        center.y - size.y * 0.5f
    );
}

sf::Vector2f getViewSize(const sf::RenderWindow& window)
{
    return window.getView().getSize();
}

sf::Vector2f getRectCenter(const sf::FloatRect& rect)
{
    return sf::Vector2f(
        rect.position.x + rect.size.x * 0.5f,
        rect.position.y + rect.size.y * 0.5f
    );
}

void setSpriteOriginToCenter(sf::Sprite& sprite)
{
    sprite.setOrigin(getRectCenter(sprite.getLocalBounds()));
}

void fitSpriteInside(
    sf::Sprite& sprite,
    sf::Vector2f maxSize,
    bool allowUpscale
)
{
    const sf::FloatRect bounds = sprite.getLocalBounds();

    if (bounds.size.x <= 0.0f || bounds.size.y <= 0.0f) {
        return;
    }

    float scale = std::min(
        maxSize.x / bounds.size.x,
        maxSize.y / bounds.size.y
    );

    if (!allowUpscale) {
        scale = std::min(scale, 1.0f);
    }

    sprite.setScale(sf::Vector2f(scale, scale));
}

void setSpriteOpacity(sf::Sprite& sprite, std::uint8_t alpha)
{
    sf::Color color = sprite.getColor();
    color.a = alpha;
    sprite.setColor(color);
}

void drawBackgroundPart(
    sf::RenderWindow& window,
    sf::Texture& texture,
    bool alignBottom
)
{
    sf::Sprite sprite(texture);

    const sf::Vector2f viewTopLeft = getViewTopLeft(window);
    const sf::Vector2f viewSize = getViewSize(window);
    const sf::Vector2u textureSize = texture.getSize();

    if (textureSize.x == 0 || textureSize.y == 0) {
        return;
    }

    const float scale = std::min(
        1.0f,
        viewSize.x / static_cast<float>(textureSize.x)
    );

    sprite.setScale(sf::Vector2f(scale, scale));

    const float drawWidth =
        static_cast<float>(textureSize.x) * scale;
    const float drawHeight =
        static_cast<float>(textureSize.y) * scale;

    const float x =
        viewTopLeft.x + (viewSize.x - drawWidth) * 0.5f;

    const float y = alignBottom
        ? viewTopLeft.y + viewSize.y - drawHeight
        : viewTopLeft.y;

    sprite.setPosition(sf::Vector2f(x, y));

    window.draw(sprite);
}

std::string getNodeTextureId(const MapNode& node)
{
    if (node.state == MapNodeState::Completed) {
        if (node.type == MapNodeType::Event) {
            return "eventOutline";
        }

        return "monsterOutline";
    }

    if (node.type == MapNodeType::Event) {
        return "event";
    }

    return "monster";
}

void setRotationDegrees(sf::Transformable& transformable, float degrees)
{
    transformable.setRotation(sf::degrees(degrees));
}

void drawLine(
    sf::RenderWindow& window,
    sf::Vector2f from,
    sf::Vector2f to,
    sf::Color color
)
{
    const float dx = to.x - from.x;
    const float dy = to.y - from.y;

    const float length = std::sqrt(dx * dx + dy * dy);
    const float angle =
        std::atan2(dy, dx) * 180.0f / 3.1415926f;

    sf::RectangleShape line(sf::Vector2f(length, 5.0f));
    line.setOrigin(sf::Vector2f(0.0f, 2.5f));
    line.setPosition(from);
    setRotationDegrees(line, angle);
    line.setFillColor(color);

    window.draw(line);
}

}

MapScene::MapScene(GameContext& context)
    : Scene(context)
{
    if (context.runState.mapNodes.empty()) {
        mapSystem_.generateRouteMap(
    context.runState,
    context.events
);


        context.runState.bossEnemyId =
            context.enemies.chooseRandomBossId(
                context.runState.rng
            );
    } else {
        mapSystem_.refreshNodeStates(context.runState);

        if (context.runState.bossEnemyId == 0) {
            context.runState.bossEnemyId =
                context.enemies.chooseRandomBossId(
                    context.runState.rng
                );
        }
    }
}

void MapScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    if (transition_.target != SceneType::None) {
        return;
    }

    sf::Vector2i pixelPosition;

    if (!readLeftClickPosition(event, pixelPosition)) {
        return;
    }

    const sf::Vector2f mousePos =
        window.mapPixelToCoords(pixelPosition);

    const int nodeIndex = getNodeIndexAtPosition(mousePos);

    if (nodeIndex >= 0) {
        enterNode(nodeIndex);
    }
}

void MapScene::update(float)
{
}

void MapScene::draw(sf::RenderWindow& window)
{
    const sf::Vector2f viewTopLeft = getViewTopLeft(window);
    const sf::Vector2f viewSize = getViewSize(window);

    sf::RectangleShape background(viewSize);
    background.setPosition(viewTopLeft);
    background.setFillColor(sf::Color(24, 26, 36));
    window.draw(background);

    if (context.resources.hasTexture("mapTop")) {
        drawBackgroundPart(
            window,
            context.resources.getTexture("mapTop"),
            false
        );
    }

    const sf::Font& font = context.resources.getFont("zh-R");

    std::ostringstream status;
    status << "Map"
           << "    HP: " << context.runState.player.hp
           << " / " << context.runState.player.maxHp
           << "    Gold: " << context.runState.gold
           << "    Floor: " << context.runState.floor
           << "    Click the highlighted icon to continue";

    sf::Text statusText = makeText(font, status.str(), 28);
    statusText.setFillColor(sf::Color(245, 240, 220));
    statusText.setPosition(sf::Vector2f(
        viewTopLeft.x + 60.0f,
        viewTopLeft.y + 28.0f
    ));
    window.draw(statusText);

    const int total =
        static_cast<int>(context.runState.mapNodes.size());

    for (const MapNode& node : context.runState.mapNodes) {
        const sf::Vector2f from =
            getNodePosition(node.index, total);

        for (int nextIndex : node.nextIndices) {
            if (
                nextIndex < 0 ||
                nextIndex >= static_cast<int>(context.runState.mapNodes.size())
            ) {
                continue;
            }

            const sf::Vector2f to =
                getNodePosition(nextIndex, total);

            sf::Color lineColor(170, 170, 100, 150);
            lineColor.a=50;

            if (node.state == MapNodeState::Completed) {
                lineColor = sf::Color(220, 210, 170, 220);
            }

            drawLine(window, from, to, lineColor);
        }
    }


    for (const MapNode& node : context.runState.mapNodes) {
        sf::Sprite sprite = makeNodeSprite(node, total);
        window.draw(sprite);
    }
}

SceneTransition MapScene::getTransition() const
{
    return transition_;
}

int MapScene::getLayerNodeCount(int layer) const
{
    int count = 0;

    for (const MapNode& node : context.runState.mapNodes) {
        if (node.layer == layer) {
            ++count;
        }
    }

    return count;
}

int MapScene::getMaxLayer() const
{
    int maxLayer = 0;

    for (const MapNode& node : context.runState.mapNodes) {
        maxLayer = std::max(maxLayer, node.layer);
    }

    return maxLayer;
}

sf::Vector2f MapScene::getNodePosition(int index, int) const
{
    if (
        index < 0 ||
        index >= static_cast<int>(context.runState.mapNodes.size())
    ) {
        return sf::Vector2f(960.0f, 540.0f);
    }

    const MapNode& node = context.runState.mapNodes[index];

    return sf::Vector2f(node.mapX, node.mapY);
}



float MapScene::getNodeDisplaySize(
    const MapNode& node,
    int total
) const
{
    float size = 110.0f;

    if (isBossNode(node, total)) {
        size = 350.0f;
    }

    return size;
}



sf::Sprite MapScene::makeNodeSprite(
    const MapNode& node,
    int total
) const
{
    const std::string textureId = getNodeTextureId(node, total);


    sf::Sprite sprite(context.resources.getTexture(textureId));

    setSpriteOriginToCenter(sprite);

    const float displaySize = getNodeDisplaySize(node, total);


    fitSpriteInside(
    sprite,
    sf::Vector2f(displaySize, displaySize),
    isBossNode(node, total)
);

    sprite.setPosition(getNodePosition(node.index, total));

    if (node.state == MapNodeState::Locked) {
        setSpriteOpacity(sprite, 110);
    } else if (node.state == MapNodeState::Available) {
        setSpriteOpacity(sprite, 255);
    } else {
        setSpriteOpacity(sprite, 230);
    }

    return sprite;
}

int MapScene::getNodeIndexAtPosition(sf::Vector2f mousePos) const
{
    const int total =
        static_cast<int>(context.runState.mapNodes.size());

    for (const MapNode& node : context.runState.mapNodes) {
        if (node.state != MapNodeState::Available) {
            continue;
        }

        sf::Sprite sprite = makeNodeSprite(node, total);

        if (sprite.getGlobalBounds().contains(mousePos)) {
            return node.index;
        }
    }

    return -1;
}


void MapScene::enterNode(int nodeIndex)
{
    ErrorCode error =
        mapSystem_.selectNode(context.runState, nodeIndex);

    if (error != ErrorCode::OK) {
        return;
    }

    MapNode& node = context.runState.mapNodes[nodeIndex];

    context.runState.floor += 1;

    if (node.type == MapNodeType::Event) {
        transition_.target = SceneType::Event;
        transition_.eventId = node.eventId;
        transition_.mapNodeIndex = node.index;
        return;
    }

    if (node.type == MapNodeType::Campfire) {
        transition_.target = SceneType::Campfire;
        transition_.mapNodeIndex = node.index;
        return;
    }

    if (node.type == MapNodeType::Shop) {
        transition_.target = SceneType::Shop;
        transition_.mapNodeIndex = node.index;
        return;
    }

    EnemyId enemyId = 0;

    const bool finalNode = node.nextIndices.empty();

    if (finalNode && context.runState.bossEnemyId > 0) {
        enemyId = context.runState.bossEnemyId;
    } else {
        enemyId = context.enemies.chooseEnemyIdByFloor(
            context.runState.floor,
            context.runState.rng
        );
    }

    context.runState.currentEnemyId = enemyId;

    transition_.target = SceneType::Combat;
    transition_.enemyId = enemyId;
    transition_.mapNodeIndex = node.index;

}
bool MapScene::isBossNode(
    const MapNode& node,
    int
) const
{
    return node.type == MapNodeType::Combat &&
           node.nextIndices.empty() &&
           context.runState.bossEnemyId > 0;
}


std::string MapScene::getNodeTextureId(
    const MapNode& node,
    int total
) const
{
    if (isBossNode(node, total)) {
        const EnemyDef& boss =
            context.enemies.get(context.runState.bossEnemyId);

        if (
            !boss.mapTextureId.empty() &&
            context.resources.hasTexture(boss.mapTextureId)
        ) {
            return boss.mapTextureId;
        }

        if (
            !boss.textureId.empty() &&
            context.resources.hasTexture(boss.textureId)
        ) {
            return boss.textureId;
        }
    }

    if (node.state == MapNodeState::Completed) {
        if (node.type == MapNodeType::Event) {
            return "eventOutline";
        }

        if (node.type == MapNodeType::Campfire) {
            return "campfireOutline";
        }

        if (node.type == MapNodeType::Shop) {
            return "shopOutline";
        }

        return "monsterOutline";
    }

    if (node.type == MapNodeType::Event) {
        return "event";
    }

    if (node.type == MapNodeType::Campfire) {
        return "campfire";
    }

    if (node.type == MapNodeType::Shop) {
        return "shop";
    }

    return "monster";
}


