#ifndef SPIRELIKE_CAMPFIRESCENE_HPP
#define SPIRELIKE_CAMPFIRESCENE_HPP

#include "core/Scene.hpp"
#include "core/SceneTransition.hpp"
#include "system/MapSystem.hpp"

#include <SFML/Graphics.hpp>

#include "ui/Button.hpp"

class CampfireScene : public Scene {
public:
    explicit CampfireScene(GameContext& context);

    void handleEvent(
        const sf::Event& event,
        const sf::RenderWindow& window
    ) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) override;

    SceneTransition getTransition() const override;

private:
    void rest();

private:
    MapSystem mapSystem_;
    SceneTransition transition_;
    Button restButton_;
    bool used_ = false;
};

#endif // SPIRELIKE_CAMPFIRESCENE_HPP
