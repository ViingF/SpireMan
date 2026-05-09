
#ifndef SPIRELIKE_CHARACTERSELECTSCENE_HPP
#define SPIRELIKE_CHARACTERSELECTSCENE_HPP

#include "core/Scene.hpp"
#include "core/GameContext.hpp"
#include "core/SceneTransition.hpp"

#include "ui/Button.hpp"

#include <SFML/Graphics.hpp>

class CharacterSelectScene : public Scene {
public:
    explicit CharacterSelectScene(
        GameContext& context
    );

    void handleEvent(
        const sf::Event& event,
        const sf::RenderWindow& window
    ) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) override;

    SceneTransition getTransition() const override;

private:
    Button ironcladButton;
    RectangleShape ironcladPortrait;

    RectangleShape lockedButtons[3];

    Button backButton;

    SceneTransition transition;
};

#endif //SPIRELIKE_CHARACTERSELECTSCENE_HPP