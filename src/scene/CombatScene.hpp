#ifndef SPIRELIKE_COMBATSCENE_HPP
#define SPIRELIKE_COMBATSCENE_HPP

#include "core/Scene.hpp"
#include "system/CombatSystem.hpp"
#include "ui/Button.hpp"
#include "ui/CardPileOverlay.hpp"
#include "ui/CardView.hpp"
#include "ui/EnemyGroupView.hpp"
#include "ui/EnergyView.hpp"
#include "ui/HealthBar.hpp"
#include "ui/IntentView.hpp"
#include "ui/StateView.hpp"

#include <optional>
#include <string>
#include <vector>

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


    void handlePileButtons(
        const sf::Event& event,
        const sf::RenderWindow& window
    );

    void updateBattleTransition();

    void drawPlayerInfo(sf::RenderWindow& window) const;

    void drawPlayerStatus(
        sf::RenderWindow& window,
        sf::Font& font
    ) const;

    void drawBackground(sf::RenderWindow& window) const;

    void drawPanelHeart(sf::RenderWindow& window) const;

    void drawPanelGoldBag(sf::RenderWindow& window) const;

    void drawDeck(sf::RenderWindow& window) const;

    void drawMap(sf::RenderWindow& window) const;

    void drawFloor(sf::RenderWindow& window) const;

    void drawSettings(sf::RenderWindow& window) const;

    void drawBurningBlood(sf::RenderWindow& window) const;

    void drawTop(sf::RenderWindow& window) const;

    void drawHand(
        sf::RenderWindow& window,
        sf::Font& font
    ) const;

    void drawEnemies(
        sf::RenderWindow& window,
        sf::Font &font
    ) const;

private:
    CombatSystem combatSystem_;
    EnemyDatabase enemyDatabase_;

    Button endTurnButton_;
    Button drawPileButton_;
    Button discardPileButton_;
    Button exhaustPileButton_;

    CardPileOverlay pileOverlay_;

    EnemyGroupView enemyGroupView_;

    std::vector<CardView> cardViews_;
    std::vector<std::string> enemyTextureIds_;

    int hoveredCardIndex_ = -1;
    int selectedHandIndex_ = -1;

    SceneTransition transition_;

    EncounterDef encounterDef_;
    GameContext& context_;
};

#endif
