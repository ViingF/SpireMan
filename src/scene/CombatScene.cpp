
#include "CombatScene.hpp"

CombatScene::CombatScene(
    GameContext& context,
    const EnemyDef& enemyDef
)
    : Scene(context),
      endTurnButton(
          sf::Vector2f(1550.f, 850.f),
          sf::Vector2f(250.f, 100.f),
          "End Turn"
      )
{
    combatSystem_.startCombat(
        context.runState,
        enemyDef,
        context.cards
    );
}

void CombatScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    if (
        combatSystem_.getBattleResult()
        != BattleResult::Ongoing
    )
    {
        return;
    }

    endTurnButton.handleEvent(event, window);

    if (endTurnButton.wasClicked())
    {
        combatSystem_.endPlayerTurn();

        endTurnButton.reset();
    }
}

void CombatScene::update(float dt)
{
    BattleResult result =
        combatSystem_.getBattleResult();

    if (result == BattleResult::Ongoing)
    {
        return;
    }

    if (!combatSystem_.hasCommittedResult())
    {
        combatSystem_.commitResultToRunState(
            context.runState
        );
    }

    if (result == BattleResult::Victory)
    {
        transition.target = SceneType::Reward;
        transition.battleResult = BattleResult::Victory;
    }
    else if (result == BattleResult::Defeat)
    {
        transition.target = SceneType::End;
        transition.battleResult = BattleResult::Defeat;
    }
}

void CombatScene::draw(
    sf::RenderWindow& window
)
{
    window.clear(
        sf::Color(35, 35, 45)
    );

    endTurnButton.draw(window);

}

SceneTransition CombatScene::getTransition() const
{
    return transition;
}
