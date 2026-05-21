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

    startButton(
    sf::Vector2f(1650.f, 800.f),
    sf::Vector2f(320.f, 90.f),
    context.resources.getFont("zh-R"),
    "开始"
    ),

    backButton(
    sf::Vector2f(0, 800.f),
    sf::Vector2f(220.f, 80.f),
    context.resources.getFont("zh-R"),
    "返回"
    )
{
    ironcladButton.setTexture(context.resources.getTexture("ironcladButton"));
    ironcladPortrait.setTexture(&context.resources.getTexture("ironcladPortrait"));
    ironcladPortrait.setSize({1920,1080});
    for (int i = 0; i < 3; i++) {
        lockedButtons[i].setTexture(&context.resources.getTexture("lockedButton"));
        lockedButtons[i].setSize({120.f,120.f});
        lockedButtons[i].setPosition({700.f+i*200.f,800.f});
    }
    startButton.setTexture(context.resources.getTexture("confirmButton"));
    backButton.setTexture(context.resources.getTexture("cancelButton"));
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

    startButton.handleEvent(
        event,
        window
    );

    backButton.handleEvent(
        event,
        window
    );

    if (ironcladButton.wasClicked())
    {
        context.audio.playSound("Click");

        ironcladSelected = true;

        ironcladButton.reset();
    }

    if (startButton.wasClicked())
    {
        if (ironcladSelected)
        {
            context.audio.playSound("Click");

            transition.target = SceneType::Map;
        }

        startButton.reset();
    }

    if (backButton.wasClicked())
    {
        context.audio.playSound("Click");
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
    startButton.draw(window);
    backButton.draw(window);

}

SceneTransition
CharacterSelectScene::getTransition() const
{
    return transition;
}

void CharacterSelectScene::resetTransition() {
    transition = SceneTransition{};
}