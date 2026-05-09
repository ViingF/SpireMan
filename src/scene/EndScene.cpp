#include "scene/EndScene.hpp"

EndScene::EndScene(GameContext& context, BattleResult result)
    : Scene(context),
      result(result),
      menuButton(
          sf::Vector2f(760.f, 520.f),
          sf::Vector2f(400.f, 100.f),
          context.resources.getFont("zh-R"),
          "Menu"
      ),
      quitButton(
          sf::Vector2f(760.f, 660.f),
          sf::Vector2f(400.f, 100.f),
          context.resources.getFont("zh-R"),
          "Quit"
      )
{
}

void EndScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    menuButton.handleEvent(event, window);
    quitButton.handleEvent(event, window);

    if (menuButton.wasClicked())
    {
        transition.target = SceneType::Menu;
        menuButton.reset();
    }

    if (quitButton.wasClicked())
    {
        const_cast<sf::RenderWindow&>(window).close();
        quitButton.reset();
    }
}

void EndScene::update(float dt)
{
}

void EndScene::draw(sf::RenderWindow& window)
{
    if (result == BattleResult::Victory)
    {
        window.clear(sf::Color(30, 60, 40));
    }
    else
    {
        window.clear(sf::Color(60, 30, 30));
    }

    menuButton.draw(window);
    quitButton.draw(window);
}

SceneTransition EndScene::getTransition() const
{
    return transition;
}
