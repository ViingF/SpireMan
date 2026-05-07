#include "MapScene.hpp"

#include "config/Constants.hpp"

#include <SFML/Config.hpp>

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

namespace {

sf::Text makeText(
    const sf::Font& font,
    const std::string& content,
    unsigned int size
)
{
#if SFML_VERSION_MAJOR >= 3
    sf::Text text(font, content, size);
#else
    sf::Text text;
    text.setFont(font);
    text.setString(content);
    text.setCharacterSize(size);
#endif
    return text;
}

bool readLeftClickPosition(
    const sf::Event& event,
    sf::Vector2i& pixelPosition
)
{
#if SFML_VERSION_MAJOR >= 3
    if (const auto* mouse =
            event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouse->button == sf::Mouse::Button::Left) {
            pixelPosition = mouse->position;
            return true;
        }
    }
#else
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        pixelPosition = sf::Vector2i(
            event.mouseButton.x,
            event.mouseButton.y
        );
        return true;
    }
#endif

    return false;
}

float distance(sf::Vector2f a, sf::Vector2f b)
{
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

void setRotationDegrees(sf::Transformable& shape, float degrees)
{
#if SFML_VERSION_MAJOR >= 3
    shape.setRotation(sf::degrees(degrees));
#else
    shape.setRotation(degrees);
#endif
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
    const float angle = std::atan2(dy, dx) * 180.0f / 3.1415926f;

    sf::RectangleShape line(sf::Vector2f(length, 6.0f));
    line.setOrigin(sf::Vector2f(0.0f, 3.0f));
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
        mapSystem_.generateSingleRoute(
            context.runState,
            context.events
        );
    } else {
        mapSystem_.refreshNodeStates(context.runState);
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
    sf::RectangleShape background(sf::Vector2f(1920.0f, 1080.0f));
    background.setFillColor(sf::Color(24, 26, 36));
    window.draw(background);

    const sf::Font& font = context.resources.getFont("zh-R");

    sf::Text title = makeText(font, "Single Route Map", 48);
    title.setFillColor(sf::Color::White);
    title.setPosition(sf::Vector2f(120.0f, 70.0f));
    window.draw(title);

    std::ostringstream info;
    info << "HP: " << context.runState.player.hp
         << " / " << context.runState.player.maxHp
         << "    Gold: " << context.runState.gold
         << "    Floor: " << context.runState.floor;

    sf::Text infoText = makeText(font, info.str(), 28);
    infoText.setFillColor(sf::Color(220, 220, 220));
    infoText.setPosition(sf::Vector2f(120.0f, 140.0f));
    window.draw(infoText);

    const int total =
        static_cast<int>(context.runState.mapNodes.size());

    for (int i = 0; i + 1 < total; ++i) {
        const sf::Vector2f a = getNodePosition(i, total);
        const sf::Vector2f b = getNodePosition(i + 1, total);

        sf::Color lineColor(80, 80, 95);

        if (context.runState.mapNodes[i].state ==
            MapNodeState::Completed) {
            lineColor = sf::Color(120, 170, 120);
        }

        drawLine(window, a, b, lineColor);
    }

    for (const MapNode& node : context.runState.mapNodes) {
        const sf::Vector2f pos = getNodePosition(node.index, total);

        sf::CircleShape circle(46.0f);
        circle.setOrigin(sf::Vector2f(46.0f, 46.0f));
        circle.setPosition(pos);

        if (node.state == MapNodeState::Completed) {
            circle.setFillColor(sf::Color(80, 150, 90));
        } else if (node.state == MapNodeState::Available) {
            circle.setFillColor(sf::Color(210, 160, 70));
        } else {
            circle.setFillColor(sf::Color(70, 70, 80));
        }

        circle.setOutlineThickness(4.0f);
        circle.setOutlineColor(sf::Color(230, 230, 230));
        window.draw(circle);

        std::string label;
        if (node.type == MapNodeType::Combat) {
            label = "Battle";
        } else {
            label = "Event";
        }

        sf::Text labelText = makeText(font, label, 22);
        labelText.setFillColor(sf::Color::White);
        labelText.setPosition(sf::Vector2f(pos.x - 36.0f, pos.y - 14.0f));
        window.draw(labelText);

        sf::Text indexText = makeText(
            font,
            std::to_string(node.index + 1),
            20
        );
        indexText.setFillColor(sf::Color(230, 230, 230));
        indexText.setPosition(sf::Vector2f(pos.x - 8.0f, pos.y + 50.0f));
        window.draw(indexText);
    }

    sf::Text tip = makeText(
        font,
        "Click the highlighted node to continue.",
        28
    );
    tip.setFillColor(sf::Color(200, 200, 210));
    tip.setPosition(sf::Vector2f(120.0f, 950.0f));
    window.draw(tip);
}

SceneTransition MapScene::getTransition() const
{
    return transition_;
}

sf::Vector2f MapScene::getNodePosition(int index, int total) const
{
    const float startX = 260.0f;
    const float endX = 1660.0f;
    const float centerY = 560.0f;

    if (total <= 1) {
        return sf::Vector2f(startX, centerY);
    }

    const float step = (endX - startX) / static_cast<float>(total - 1);
    const float x = startX + step * static_cast<float>(index);

    float y = centerY;

    if (index != 0 && index != total - 1) {
        y += (index % 2 == 0) ? -110.0f : 110.0f;
    }

    return sf::Vector2f(x, y);
}

int MapScene::getNodeIndexAtPosition(sf::Vector2f mousePos) const
{
    const int total =
        static_cast<int>(context.runState.mapNodes.size());

    for (const MapNode& node : context.runState.mapNodes) {
        const sf::Vector2f pos = getNodePosition(node.index, total);

        if (distance(mousePos, pos) <= 54.0f) {
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

    EnemyId enemyId = context.enemies.chooseEnemyIdByFloor(
        context.runState.floor,
        context.runState.rng
    );

    context.runState.currentEnemyId = enemyId;

    transition_.target = SceneType::Combat;
    transition_.enemyId = enemyId;
    transition_.mapNodeIndex = node.index;
}