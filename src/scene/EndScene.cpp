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
    background.setSize({1920,1080});
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
        context.audio.playSound("Click");
        transition.target = SceneType::Menu;
        menuButton.reset();
    }

    if (quitButton.wasClicked())
    {
        context.audio.playSound("Click");
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
        window.clear();
    }
    else
    {
        background.setTexture(&context.resources.getTexture("death"));
        window.clear();
    }

    window.draw(background);
    menuButton.draw(window);
    quitButton.draw(window);
}

SceneTransition EndScene::getTransition() const
{
    return transition;
}

void EndScene::resetTransition() {
    transition = SceneTransition{};
}