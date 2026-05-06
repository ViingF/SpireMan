
#ifndef SPIRELIKE_REWARDSCENE_HPP
#define SPIRELIKE_REWARDSCENE_HPP


#include "core/Scene.hpp"
#include "core/GameContext.hpp"
#include "core/SceneTransition.hpp"

#include "ui/Button.hpp"

#include "model/CardDef.hpp"
#include "model/CardInstance.hpp"

#include <SFML/Graphics.hpp>

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

private:
    void chooseCard(CardId cardId);

private:
    Button cardButton1;
    Button cardButton2;
    Button cardButton3;

    CardId rewardCard1 = 1;
    CardId rewardCard2 = 2;
    CardId rewardCard3 = 3;

    SceneTransition transition;
};

#endif //SPIRELIKE_REWARDSCENE_HPP