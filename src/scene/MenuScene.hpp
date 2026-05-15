
#ifndef SPIRELIKE_MENUSCENE_HPP
#define SPIRELIKE_MENUSCENE_HPP

#include "core/Scene.hpp"
#include "core/GameContext.hpp"
#include "core/SceneTransition.hpp"
#include "ui/Button.hpp"

#include <SFML/Graphics.hpp>

class MenuScene : public Scene {
public:
    explicit MenuScene(GameContext& context);

    void handleEvent(
        const sf::Event& event,
        const sf::RenderWindow& window
    ) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) override;

    SceneTransition getTransition() const override;

private:
    Button startButton;
    Button quitButton;
    Button loadButton;

    SceneTransition transition;
    Sprite background;

};

#endif //SPIRELIKE_MENUSCENE_HPP