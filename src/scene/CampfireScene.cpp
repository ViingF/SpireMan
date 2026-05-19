#include "CampfireScene.hpp"

#include "config/Constants.hpp"

#include <algorithm>
#include <sstream>
#include <string>

#include "ui/TextUtils.hpp"

namespace {

    sf::Text makeText(
        const sf::Font& font,
        const std::string& content,
        unsigned int size
    ) {
        return sf::Text(font, TextUtils::fromUtf8(content), size);
    }

}

CampfireScene::CampfireScene(GameContext& context)
    : Scene(context),
      restButton_(
          sf::Vector2f(1180.0f, 450.0f),
          sf::Vector2f(520.0f, 400.0f),
          context.resources.getFont("zh-R"),
          ""
      )
{
    restButton_.setTexture(context.resources.getTexture("Sleep"));
    }


void CampfireScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    if (transition_.target != SceneType::None || used_) {
        return;
    }

    restButton_.handleEvent(event, window);

    if (restButton_.wasClicked()) {
        restButton_.reset();
        rest();
    }
}


void CampfireScene::update(float)
{
}

void CampfireScene::draw(sf::RenderWindow& window)
{

    if (context.resources.hasTexture("Campfire")) {
        sf::Sprite campfire(context.resources.getTexture("Campfire"));

        const sf::Vector2u textureSize =
            context.resources.getTexture("Campfire").getSize();

        if (textureSize.x > 0 && textureSize.y > 0) {

            const float scale = std::min(
                1920 / static_cast<float>(textureSize.x),
                1080 / static_cast<float>(textureSize.y)
            );

            campfire.setScale(sf::Vector2f(scale, scale));

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

    restButton_.draw(window);

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
