
#ifndef SPIRELIKE_COMBATSCENE_HPP
#define SPIRELIKE_COMBATSCENE_HPP
#include "core/Scene.hpp"
#include "system/CombatSystem.hpp"
#include "ui/Button.hpp"
#include "ui/CardPileOverlay.hpp"

class CombatScene : public Scene {
public:
    CombatScene(
    GameContext& context,
    const EncounterDef& encounterDef
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
    Button endTurnButton;
    SceneTransition transition;
    Button winButton;
    Button drawPileButton_;
    Button discardPileButton_;
    CardPileOverlay pileOverlay_;

};
#endif //SPIRELIKE_COMBATSCENE_HPP