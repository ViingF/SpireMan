
#include "CombatScene.hpp"

CombatScene::CombatScene(
    GameContext& context,
    const EnemyDef& enemyDef
)
    : Scene(context),
      endTurnButton(
          sf::Vector2f(1550.f, 850.f),
          sf::Vector2f(250.f, 100.f),
          context.resources.getFont("zh-R"),
          "End Turn"
      ),
    winButton(sf::Vector2f(155.f, 850.f),
    sf::Vector2f(250.f, 200.f),
    context.resources.getFont("zh-R"),
    "win"
          ),
    drawPileButton_(
    sf::Vector2f(1550.f, 650.f),
    sf::Vector2f(250.f, 70.f),
    context.resources.getFont("zh-R"),
    "Draw Pile"
),

    discardPileButton_(
    sf::Vector2f(1550.f, 735.f),
    sf::Vector2f(250.f, 70.f),
    context.resources.getFont("zh-R"),
    "Discard"
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

    if (pileOverlay_.handleEvent(event, window)) {
        return;
    }

    endTurnButton.handleEvent(event, window);
    winButton.handleEvent(event, window);
    drawPileButton_.handleEvent(event, window);
    discardPileButton_.handleEvent(event, window);

    if (drawPileButton_.wasClicked()) {
        const CombatDeck& deck = combatSystem_.getDeck();

        pileOverlay_.open(
            "Draw Pile",
            &deck.drawPile
        );

        drawPileButton_.reset();
        return;
    }

    if (discardPileButton_.wasClicked()) {
        const CombatDeck& deck = combatSystem_.getDeck();

        pileOverlay_.open(
            "Discard Pile",
            &deck.discardPile
        );

        discardPileButton_.reset();
        return;
    }

    if (endTurnButton.wasClicked())
    {
        combatSystem_.endPlayerTurn();
        endTurnButton.reset();
    }

    if (winButton.wasClicked()) {
        transition.target = SceneType::Reward;
        transition.battleResult = BattleResult::Victory;
        winButton.reset();
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
    winButton.draw(window);
    drawPileButton_.draw(window);
    discardPileButton_.draw(window);
    pileOverlay_.draw(
        window,
        context.resources.getFont("zh-R"),
        context.cards
    );


}

SceneTransition CombatScene::getTransition() const
{
    return transition;
}
