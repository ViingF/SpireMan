#include "CharacterSelectScene.hpp"

CharacterSelectScene::CharacterSelectScene(
    GameContext& context
)
    : Scene(context),

      ironcladButton(
          sf::Vector2f(500.f, 600.f),
          sf::Vector2f(90.f, 90.f),
          "Ironclad"
      ),

      backButton(
          sf::Vector2f(760.f, 600.f),
          sf::Vector2f(90.f, 90.f),
          "Back"
      )
{
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

    // 选择角色
    if (ironcladButton.wasClicked())
    {
        transition.target =
            SceneType::Map;

        ironcladButton.reset();
    }

    // 返回菜单
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

    ironcladButton.draw(window);

    backButton.draw(window);
}

SceneTransition
CharacterSelectScene::getTransition() const
{
    return transition;
}
