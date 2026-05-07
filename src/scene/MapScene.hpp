
#ifndef SPIRELIKE_MAPSCENE_HPP
#define SPIRELIKE_MAPSCENE_HPP

#include "core/Scene.hpp"
#include "core/SceneTransition.hpp"
#include "system/MapSystem.hpp"

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
    sf::Vector2f getNodePosition(int index, int total) const;
    int getNodeIndexAtPosition(sf::Vector2f mousePos) const;

    void enterNode(int nodeIndex);

private:
    MapSystem mapSystem_;
    SceneTransition transition_;
};
#endif //SPIRELIKE_MAPSCENE_HPP