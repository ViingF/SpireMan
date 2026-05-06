
#ifndef SPIRELIKE_MAPSCENE_HPP
#define SPIRELIKE_MAPSCENE_HPP

#include "core/Scene.hpp"
#include "core/GameContext.hpp"
#include "core/SceneTransition.hpp"
#include "ui/Button.hpp"

#include <SFML/Graphics.hpp>

class MapScene : public Scene {
public:
    explicit MapScene(GameContext& context);

    void handleEvent(
        const sf::Event& event,
        const sf::RenderWindow& window
    ) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) override;

    SceneTransition getTransition() const override;

private:
    Button combatButton;
    Button backButton;

    SceneTransition transition;
};
#endif //SPIRELIKE_MAPSCENE_HPP