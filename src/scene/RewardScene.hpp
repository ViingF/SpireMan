
#ifndef SPIRELIKE_REWARDSCENE_HPP
#define SPIRELIKE_REWARDSCENE_HPP


#include "core/Scene.hpp"
#include "core/GameContext.hpp"
#include "core/SceneTransition.hpp"

#include "ui/Button.hpp"

#include "model/CardDef.hpp"
#include "model/CardInstance.hpp"

#include <SFML/Graphics.hpp>

#include "ui/CardView.hpp"

class RewardScene : public Scene {
public:
    explicit RewardScene(GameContext& context);

    void handleEvent(
        const sf::Event& event,
        const sf::RenderWindow& window
    ) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) override;

    SceneTransition getTransition() const override;
    void resetTransition() override;

private:
    void chooseCard(CardId cardId);
    void finishRewardAndContinueMap();
    const sf::Texture& getCardTemplateTexture(CardType type);
    const sf::Texture* getCardArtTexture(const CardDef& cardDef);
    CardRenderTextures getCardRenderTextures(const CardDef& cardDef);
    void drawReward(sf::RenderWindow &window,sf::Font &font);




private:

    SceneTransition transition;

    std::vector<CardId> rewardCardIds_;
    std::vector<CardView> rewardCardViews_;
    Button mapIconButton_;
    Button LeapButton;

    GameContext context_;
};

#endif //SPIRELIKE_REWARDSCENE_HPP