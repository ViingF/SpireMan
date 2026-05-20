#ifndef SPIRELIKE_SHOPSCENE_HPP
#define SPIRELIKE_SHOPSCENE_HPP

#include "core/Scene.hpp"
#include "core/SceneTransition.hpp"
#include "system/MapSystem.hpp"

#include <SFML/Graphics.hpp>

#include <vector>

#include "CardRemoveScene.hpp"
#include "ui/Button.hpp"
#include "ui/ShopView.hpp"

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
    void resetTransition() override;

private:

    void buyCard(int index);
    void leaveShop();


    int getRemoveCost() const;
    int getRemoveCardsPerPage() const;
    int getRemovePageCount() const;
    void clampRemovePage();

    void startRemoveCard();
    void removeCardByDeckIndex(int deckIndex);

    const sf::Texture& getCardTemplateTexture(CardType type) const;
    const sf::Texture* getCardArtTexture(const CardDef& cardDef) const;
    CardRenderTextures getCardRenderTextures(const CardDef& cardDef) const;
    std::vector<ShopCardViewData> buildShopCardViewData() const;

    void finishRemoveCardFromScene();



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
    Button mapIconButton_;

    ShopView shopView_;
    int pendingShopRemoveCost_ = 0;
    std::unique_ptr<CardRemoveScene> removeCardScene_;



};

#endif // SPIRELIKE_SHOPSCENE_HPP
