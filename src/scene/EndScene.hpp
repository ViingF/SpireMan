
#ifndef SPIRELIKE_ENDSCENE_HPP
#define SPIRELIKE_ENDSCENE_HPP

#pragma once

#include "core/Scene.hpp"
#include "core/GameContext.hpp"
#include "core/SceneTransition.hpp"
#include "ui/Button.hpp"

#include <SFML/Graphics.hpp>

class EndScene : public Scene {
public:
    EndScene(GameContext& context, BattleResult result);

    void handleEvent(
        const sf::Event& event,
        const sf::RenderWindow& window
    ) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) override;

    SceneTransition getTransition() const override;
    void resetTransition() override;

private:
    BattleResult result;

    Button menuButton;
    Button quitButton;

    SceneTransition transition;

    RectangleShape background;
};

#endif //SPIRELIKE_ENDSCENE_HPP