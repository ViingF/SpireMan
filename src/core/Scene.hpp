#ifndef SPIRELIKE_SCENE_HPP
#define SPIRELIKE_SCENE_HPP
#include "GameContext.hpp"
#include "database/CardDatabase.hpp"
#include "system/CombatSystem.hpp"

class Scene {
public:
    explicit Scene(GameContext& context);
    virtual ~Scene() = default;

    virtual void handleEvent(
    const sf::Event& event,
    const sf::RenderWindow& window
) = 0;

    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

    virtual SceneTransition getTransition() const;

};

class CombatScene : public Scene {
public:
    CombatScene(
        GameContext& context,
        const EnemyDef& enemyDef
    );

    void handleEvent(
        const sf::Event& event,
        const sf::RenderWindow& window
    ) override;

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;

    SceneTransition getTransition() const override;

private:
    CombatSystem combatSystem_;
};

#endif //SPIRELIKE_SCENE_HPP