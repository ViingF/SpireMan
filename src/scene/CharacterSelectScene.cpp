#include "CharacterSelectScene.hpp"

CharacterSelectScene::CharacterSelectScene(
    GameContext& context
)
    : Scene(context),

      ironcladButton(
          sf::Vector2f(500.f, 800.f),
          sf::Vector2f(120.f, 120.f),
          context.resources.getFont("zh-R"),
          ""
      ),

      backButton(
          sf::Vector2f(1700.f, 800.f),
          sf::Vector2f(120.f, 120.f),
          context.resources.getFont("zh-R"),
          "Back"
      ) {
    ironcladButton.setTexture(context.resources.getTexture("ironcladButton"));
    ironcladPortrait.setTexture(&context.resources.getTexture("ironcladPortrait"));
    ironcladPortrait.setSize({1920,1080});
    for (int i = 0; i < 3; i++) {
        lockedButtons[i].setTexture(&context.resources.getTexture("lockedButton"));
        lockedButtons[i].setSize({120.f,120.f});
        lockedButtons[i].setPosition({700.f+i*200.f,800.f});
    }
}

void CharacterSelectScene::handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
)
{
    ironcladButton.handleEvent(
        event,
        window
    );

    backButton.handleEvent(
        event,
        window
    );

    if (ironcladButton.wasClicked())
    {
        transition.target =
            SceneType::Map;

        ironcladButton.reset();
    }

    if (backButton.wasClicked())
    {
        transition.target =
            SceneType::Menu;

        backButton.reset();
    }
}

void CharacterSelectScene::update(
    float dt
)
{
}

void CharacterSelectScene::draw(
    sf::RenderWindow& window
)
{
    window.clear(
        sf::Color(25, 25, 35)
    );
    window.draw(ironcladPortrait);
    for (int i = 0; i < 3; i++) {
        window.draw(lockedButtons[i]);
    }
    ironcladButton.draw(window);
    backButton.draw(window);
}

SceneTransition
CharacterSelectScene::getTransition() const
{
    return transition;
}
