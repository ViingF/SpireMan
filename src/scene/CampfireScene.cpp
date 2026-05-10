#include "CampfireScene.hpp"

#include "config/Constants.hpp"

#include <algorithm>
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

sf::FloatRect getRestButtonRect()
{
    return sf::FloatRect{
        sf::Vector2f(1180.0f, 650.0f),
        sf::Vector2f(520.0f, 110.0f)
    };
}

}

CampfireScene::CampfireScene(GameContext& context)
    : Scene(context)
{
}

void CampfireScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    if (transition_.target != SceneType::None || used_) {
        return;
    }

    sf::Vector2i pixelPosition;

    if (!readLeftClickPosition(event, pixelPosition)) {
        return;
    }

    const sf::Vector2f mousePos =
        window.mapPixelToCoords(pixelPosition);

    if (getRestButtonRect().contains(mousePos)) {
        rest();
    }
}

void CampfireScene::update(float)
{
}

void CampfireScene::draw(sf::RenderWindow& window)
{
    sf::RectangleShape background(sf::Vector2f(1920.0f, 1080.0f));
    background.setFillColor(sf::Color(24, 20, 18));
    window.draw(background);

    if (context.resources.hasTexture("campfire")) {
        sf::Sprite campfire(context.resources.getTexture("campfire"));

        const sf::Vector2u textureSize =
            context.resources.getTexture("campfire").getSize();

        if (textureSize.x > 0 && textureSize.y > 0) {
            const float targetSize = 520.0f;

            const float scale = std::min(
                targetSize / static_cast<float>(textureSize.x),
                targetSize / static_cast<float>(textureSize.y)
            );

            campfire.setScale(sf::Vector2f(scale, scale));
            campfire.setPosition(sf::Vector2f(260.0f, 300.0f));

            window.draw(campfire);
        }
    }

    const sf::Font& font = context.resources.getFont("zh-R");

    std::ostringstream status;
    status << "Campfire"
           << "    HP: " << context.runState.player.hp
           << " / " << context.runState.player.maxHp
           << "    Gold: " << context.runState.gold
           << "    Floor: " << context.runState.floor;

    sf::Text statusText = makeText(font, status.str(), 28);
    statusText.setFillColor(sf::Color(245, 240, 220));
    statusText.setPosition(sf::Vector2f(60.0f, 28.0f));
    window.draw(statusText);

    sf::Text title = makeText(font, "Campfire", 56);
    title.setFillColor(sf::Color::White);
    title.setPosition(sf::Vector2f(1120.0f, 250.0f));
    window.draw(title);

    sf::Text desc = makeText(
        font,
        "Rest and recover some HP before continuing.",
        30
    );
    desc.setFillColor(sf::Color(230, 230, 230));
    desc.setPosition(sf::Vector2f(1120.0f, 340.0f));
    window.draw(desc);

    const int healAmount = std::max(
        1,
        context.runState.player.maxHp * CAMPFIRE_HEAL_PERCENT / 100
    );

    std::ostringstream healText;
    healText << "Recover " << healAmount << " HP";

    sf::Text healInfo = makeText(font, healText.str(), 28);
    healInfo.setFillColor(sf::Color(240, 220, 160));
    healInfo.setPosition(sf::Vector2f(1120.0f, 410.0f));
    window.draw(healInfo);

    const sf::FloatRect rect = getRestButtonRect();

    sf::RectangleShape button(rect.size);
    button.setPosition(rect.position);
    button.setFillColor(sf::Color(120, 70, 45));
    button.setOutlineThickness(3.0f);
    button.setOutlineColor(sf::Color(240, 210, 160));
    window.draw(button);

    sf::Text buttonText = makeText(font, "Rest", 36);
    buttonText.setFillColor(sf::Color::White);
    buttonText.setPosition(sf::Vector2f(
        rect.position.x + 210.0f,
        rect.position.y + 30.0f
    ));
    window.draw(buttonText);
}

SceneTransition CampfireScene::getTransition() const
{
    return transition_;
}

void CampfireScene::rest()
{
    const int healAmount = std::max(
        1,
        context.runState.player.maxHp * CAMPFIRE_HEAL_PERCENT / 100
    );

    context.runState.player.hp = std::min(
        context.runState.player.maxHp,
        context.runState.player.hp + healAmount
    );

    mapSystem_.completeSelectedNode(context.runState);

    if (mapSystem_.isRouteFinished(context.runState)) {
        transition_.target = SceneType::End;
        transition_.battleResult = BattleResult::Victory;
    } else {
        transition_.target = SceneType::Map;
    }

    used_ = true;
}
