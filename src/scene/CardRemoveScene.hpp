#ifndef SPIRELIKE_CARDREMOVESCENE_HPP
#define SPIRELIKE_CARDREMOVESCENE_HPP

#include "../core/Scene.hpp"
#include "../core/SceneTransition.hpp"
#include "../system/EventSystem.hpp"
#include "../system/MapSystem.hpp"

#include <SFML/Graphics.hpp>

#include <string>

#include "ui/Button.hpp"
#include "ui/CardView.hpp"

enum class CardRemoveSceneMode {
    Event,
    Embedded
};

class CardRemoveScene : public Scene {
public:
    explicit CardRemoveScene(
    GameContext& context,
    CardRemoveSceneMode mode = CardRemoveSceneMode::Event
);

    bool isFinished() const;


    void handleEvent(
        const sf::Event& event,
        const sf::RenderWindow& window
    ) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) override;

    SceneTransition getTransition() const override;
    void resetTransition() override;

private:
    sf::FloatRect getCardRect(int visibleIndex) const;
    sf::FloatRect getPrevPageRect() const;
    sf::FloatRect getNextPageRect() const;

    int getCardsPerPage() const;
    int getPageCount() const;
    void clampPage();

    void chooseCardByDeckIndex(int deckIndex);
    void continueAfterCardRemoval();
    void finishNormalEventNode();
    void handleResolveResult(const EventResolveResult& result);
    const sf::Texture& getCardTemplateTexture(CardType type) const;
    const sf::Texture* getCardArtTexture(const CardDef& cardDef) const;
    CardRenderTextures getCardRenderTextures(const CardDef& cardDef) const;


private:
    EventSystem eventSystem_;
    MapSystem mapSystem_;

    SceneTransition transition_;
    Button mapIconButton_;

    int page_ = 0;
    std::string message_;
    CardRemoveSceneMode mode_ = CardRemoveSceneMode::Event;
    bool finished_ = false;

};

#endif // SPIRELIKE_CARDREMOVESCENE_HPP
