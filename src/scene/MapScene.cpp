#include "MapScene.hpp"

MapScene::MapScene(GameContext& context)
: Scene(context),
  combatButton(
      sf::Vector2f(760.f, 420.f),
      sf::Vector2f(400.f, 100.f),
      "Combat"
  ),
  backButton(
      sf::Vector2f(760.f, 560.f),
      sf::Vector2f(400.f, 100.f),
      "Back"
  )
{
}

void MapScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    combatButton.handleEvent(event, window);
    backButton.handleEvent(event, window);

    if (combatButton.wasClicked())
    {
        context.runState.floor += 1;

        EnemyId enemyId = context.enemies.chooseEnemyIdByFloor(
            context.runState.floor,
            context.runState.rng
        );

        context.runState.currentEnemyId = enemyId;

        transition.target = SceneType::Combat;
        transition.enemyId = enemyId;

        combatButton.reset();
    }

    if (backButton.wasClicked())
    {
        transition.target = SceneType::Menu;
        backButton.reset();
    }
}

void MapScene::update(float dt)
{
}

void MapScene::draw(sf::RenderWindow& window)
{
    window.clear(sf::Color(25, 35, 45));

    combatButton.draw(window);
    backButton.draw(window);
}

SceneTransition MapScene::getTransition() const
{
    return transition;
}