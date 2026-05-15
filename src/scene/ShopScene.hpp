#ifndef SPIRELIKE_SHOPSCENE_HPP
#define SPIRELIKE_SHOPSCENE_HPP

#include "core/Scene.hpp"
#include "core/SceneTransition.hpp"
#include "system/MapSystem.hpp"

#include <SFML/Graphics.hpp>

#include <vector>

#include "ui/Button.hpp"

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
    sf::FloatRect getRemoveCardRect() const;

    sf::FloatRect getDeckCardRect(int visibleIndex) const;
    sf::FloatRect getRemovePrevPageRect() const;
    sf::FloatRect getRemoveNextPageRect() const;
    sf::FloatRect getCancelRemoveRect() const;

    int getRemoveCost() const;
    int getRemoveCardsPerPage() const;
    int getRemovePageCount() const;
    void clampRemovePage();

    void startRemoveCard();
    void removeCardByDeckIndex(int deckIndex);
    void drawRemoveCardOverlay(
        sf::RenderWindow& window,
        const sf::Font& font
    );


private:
    MapSystem mapSystem_;
    SceneTransition transition_;

    std::vector<CardId> cardOffers_;
    std::vector<bool> sold_;

    std::string message_;

    bool removingCard_ = false;
    int removePage_ = 0;
    int shopRemoveCount_ = 0;

    Button removeCardButton_;
    Button leaveButton_;
    Button cancelRemoveButton_;
    Button removePrevPageButton_;
    Button removeNextPageButton_;


};

#endif // SPIRELIKE_SHOPSCENE_HPP
