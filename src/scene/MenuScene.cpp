#include "scene/MenuScene.hpp"


#include "config/Constants.hpp"

MenuScene::MenuScene(GameContext& context)
    :   Scene(context),

        startButton(
          sf::Vector2f(76.f, 820.f),
          sf::Vector2f(400.f, 100.f),
          "Start"
      ),

        quitButton(
          sf::Vector2f(76.f, 960.f),
          sf::Vector2f(400.f, 100.f),
          "Quit"
      ) ,
        background(context.resources.getTexture(
            "title"
        )) {
    Vector2u size = context.resources.getTexture(
            "title"
        ).getSize();
    float targetWidth = 1920;
    float targetHeight = 1080;
    float scaleX = targetWidth / size.x;
    float scaleY = targetHeight / size.y;

    background.setScale({scaleX, scaleY});
}

void MenuScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    startButton.handleEvent(event, window);
    quitButton.handleEvent(event, window);

    if (startButton.wasClicked())
    {
        transition.target = SceneType::CharacterSelect;

        startButton.reset();
    }

    if (quitButton.wasClicked())
    {
        const_cast<sf::RenderWindow&>(window).close();

        quitButton.reset();
    }
}

void MenuScene::update(float dt)
{

}

void MenuScene::draw(sf::RenderWindow& window)
{
    window.clear();

    window.draw(background);

    startButton.draw(window);
    quitButton.draw(window);
}

SceneTransition MenuScene::getTransition() const
{
    return transition;
}
