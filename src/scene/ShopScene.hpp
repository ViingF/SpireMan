#ifndef SPIRELIKE_SHOPSCENE_HPP
#define SPIRELIKE_SHOPSCENE_HPP

#include "core/Scene.hpp"
#include "core/SceneTransition.hpp"
#include "system/MapSystem.hpp"

#include <SFML/Graphics.hpp>

#include <vector>

class ShopScene : public Scene {
public:
    explicit ShopScene(GameContext& context);

    void handleEvent(
        const sf::Event& event,
        const sf::RenderWindow& window
    ) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) override;

    SceneTransition getTransition() const override;

private:
    sf::FloatRect getCardRect(int index) const;
    sf::FloatRect getLeaveRect() const;

    void buyCard(int index);
    void leaveShop();

private:
    MapSystem mapSystem_;
    SceneTransition transition_;

    std::vector<CardId> cardOffers_;
    std::vector<bool> sold_;

    std::string message_;
};

#endif // SPIRELIKE_SHOPSCENE_HPP
