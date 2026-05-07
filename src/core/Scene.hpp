#ifndef SPIRELIKE_SCENE_HPP
#define SPIRELIKE_SCENE_HPP
#include "GameContext.hpp"
#include "SceneTransition.hpp"
#include "database/CardDatabase.hpp"


class Scene {
public:
    Scene(GameContext& context)
    : context(context)
    {}
    virtual ~Scene() = default;

    virtual void handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
) = 0;

    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

    virtual SceneTransition getTransition() const=0;

protected:
    GameContext context;

};


#endif //SPIRELIKE_SCENE_HPP