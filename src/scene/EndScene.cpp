#include "scene/EndScene.hpp"

EndScene::EndScene(GameContext& context, BattleResult result)
    : Scene(context),
      result(result),
      menuButton(
          sf::Vector2f(760.f, 520.f),
          sf::Vector2f(400.f, 100.f),
          context.resources.getFont("zh-R"),
          "主菜单"
      ),
      quitButton(
          sf::Vector2f(760.f, 660.f),
          sf::Vector2f(400.f, 100.f),
          context.resources.getFont("zh-R"),
          "退出"
      )
{
    background.setSize({1920,1080});
    menuButton.setTexture(context.resources.getTexture("endTurnButton"));
    quitButton.setTexture(context.resources.getTexture("endTurnButton"));
}

void EndScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    if (!shouldShowButtons()) {
        return;
    }

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
    if (
        result == BattleResult::Victory &&
        !shouldShowButtons()
    ) {
        victoryElapsed += dt;
    }
}


void EndScene::draw(sf::RenderWindow& window)
{
    window.clear();

    if (result == BattleResult::Victory)
    {
        const int imageIndex = currentVictoryImageIndex();

        background.setTexture(
            &context.resources.getTexture(
                "end" + std::to_string(imageIndex)
            )
        );
    }
    else
    {
        background.setTexture(&context.resources.getTexture("death"));
    }

    window.draw(background);

    if (shouldShowButtons()) {
        menuButton.draw(window);
        quitButton.draw(window);
    }
}


SceneTransition EndScene::getTransition() const
{
    return transition;
}

void EndScene::resetTransition() {
    transition = SceneTransition{};
}

bool EndScene::shouldShowButtons() const
{
    if (result != BattleResult::Victory) {
        return true;
    }

    return victoryElapsed >=
        VictoryFrameSeconds * VictoryIntroImageCount;
}

int EndScene::currentVictoryImageIndex() const
{
    if (shouldShowButtons()) {
        return 5;
    }

    const int imageIndex =
        static_cast<int>(victoryElapsed / VictoryFrameSeconds) + 1;

    if (imageIndex < 1) {
        return 1;
    }

    if (imageIndex > VictoryIntroImageCount) {
        return VictoryIntroImageCount;
    }

    return imageIndex;
}
