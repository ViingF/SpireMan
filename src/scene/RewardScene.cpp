#include "scene/RewardScene.hpp"

#include "config/Constants.hpp"
#include "system/MapSystem.hpp"
RewardScene::RewardScene(
    GameContext& context
)
    : Scene(context),

      cardButton1(
          sf::Vector2f(300.f, 400.f),
          sf::Vector2f(300.f, 500.f),
          context.resources.getFont("zh-R"),
          "Strike"
      ),

      cardButton2(
          sf::Vector2f(810.f, 400.f),
          sf::Vector2f(300.f, 500.f),
          context.resources.getFont("zh-R"),
          "Defend"
      ),

      cardButton3(
          sf::Vector2f(1320.f, 400.f),
          sf::Vector2f(300.f, 500.f),
          context.resources.getFont("zh-R"),
          "Bash"
      )
{
}

void RewardScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    cardButton1.handleEvent(event, window);
    cardButton2.handleEvent(event, window);
    cardButton3.handleEvent(event, window);

    if (cardButton1.wasClicked())
    {
        chooseCard(rewardCard1);

        cardButton1.reset();
    }

    if (cardButton2.wasClicked())
    {
        chooseCard(rewardCard2);

        cardButton2.reset();
    }

    if (cardButton3.wasClicked())
    {
        chooseCard(rewardCard3);

        cardButton3.reset();
    }
}

void RewardScene::update(float dt)
{
}

void RewardScene::draw(
    sf::RenderWindow& window
)
{
    window.clear(
        sf::Color(40, 40, 60)
    );

    cardButton1.draw(window);
    cardButton2.draw(window);
    cardButton3.draw(window);
}

SceneTransition RewardScene::getTransition() const
{
    return transition;
}

void RewardScene::chooseCard(CardId cardId)
{
    if (!context.cards.exists(cardId)) {
        return;
    }

    CardInstance newCard;
    newCard.instanceId = context.runState.nextCardInstanceId;
    newCard.cardId = cardId;

    context.runState.nextCardInstanceId += 1;
    context.runState.masterDeck.push_back(newCard);

    MapSystem mapSystem;

    if (!context.runState.mapNodes.empty()) {
        mapSystem.completeSelectedNode(context.runState);

        if (mapSystem.isRouteFinished(context.runState)) {
            transition.target = SceneType::End;
            transition.battleResult = BattleResult::Victory;
        } else {
            transition.target = SceneType::Map;
        }

        return;
    }

    transition.target = SceneType::Map;
}

