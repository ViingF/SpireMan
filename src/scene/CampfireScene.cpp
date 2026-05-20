#include "CampfireScene.hpp"

#include "config/Constants.hpp"

#include <algorithm>
#include <sstream>
#include <string>

#include "ui/TextUtils.hpp"
#include "ui/TopInfoBar.hpp"

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
          ),mapIconButton_(
        sf::Vector2f(0.0f, 0.0f),
        sf::Vector2f(64.0f, 64.0f),
        context.resources.getFont("zh-R"),
        ""
    )

{
    mapIconButton_.setTexture(
    context.resources.getTexture("map")
);

    restButton_.setTexture(context.resources.getTexture("Sleep"));
    }


void CampfireScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    TopInfoBar::layoutMapButton(mapIconButton_, window);
    mapIconButton_.handleEvent(event, window);

    if (mapIconButton_.wasClicked()) {
        context.audio.playSound("Click");

        transition_.openMapPreview = true;
        transition_.target = SceneType::Map;

        mapIconButton_.reset();
        return;
    }


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

    TopInfoBar::draw(
    window,
    context,
    font,
    std::nullopt,
    true,
    true,
    false // map 图标由按钮绘制
);

    TopInfoBar::layoutMapButton(mapIconButton_, window);
    mapIconButton_.draw(window);



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

void CampfireScene::resetTransition()
{
    transition_ = SceneTransition{};
}
