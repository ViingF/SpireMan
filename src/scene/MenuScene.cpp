#include "scene/MenuScene.hpp"


#include "config/Constants.hpp"
#include "system/SaveSystem.hpp"

MenuScene::MenuScene(GameContext& context)
    :   Scene(context),

        startButton(
          sf::Vector2f(76.f, 820.f),
          sf::Vector2f(400.f, 100.f),
          context.resources.getFont("zh-R"),
          "开始"
      ),

        quitButton(
          sf::Vector2f(76.f, 960.f),
          sf::Vector2f(400.f, 100.f),
          context.resources.getFont("zh-R"),
          "Quit"
      ) ,
    loadButton(
          sf::Vector2f(76.f, 670.f),
          sf::Vector2f(400.f, 90.f),
          context.resources.getFont("zh-R"),
          "Load"
      ),
        background(context.resources.getTexture(
            "title"
        ))
{
    Vector2u size = context.resources.getTexture(
            "title"
        ).getSize();
    float targetWidth = 1920;
    float targetHeight = 1080;
    float scaleX = targetWidth / size.x;
    float scaleY = targetHeight / size.y;

    background.setScale({scaleX, scaleY});
    loadButton.setEnabled(SaveSystem::hasSave());
    startButton.setTexture(context.resources.getTexture("blank"));
    quitButton.setTexture(context.resources.getTexture("blank"));
    loadButton.setTexture(context.resources.getTexture("blank"));

}

void MenuScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    startButton.handleEvent(event, window);
    quitButton.handleEvent(event, window);
    loadButton.handleEvent(event, window);

    if (startButton.wasClicked())
    {
        context.audio.playSound("Click");

        transition.target = SceneType::CharacterSelect;

        startButton.reset();
    }

    if (quitButton.wasClicked())
    {
        context.audio.playSound("Click");

        const_cast<sf::RenderWindow&>(window).close();

        quitButton.reset();
    }

    if (loadButton.wasClicked())
    {
        context.audio.playSound("Click");

        if (SaveSystem::hasSave()) {
            transition.target = SceneType::Map;
            transition.loadGame = true;
        }

        loadButton.reset();
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
    loadButton.draw(window);
}

SceneTransition MenuScene::getTransition() const
{
    return transition;
}
