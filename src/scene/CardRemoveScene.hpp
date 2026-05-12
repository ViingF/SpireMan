#ifndef SPIRELIKE_CARDREMOVESCENE_HPP
#define SPIRELIKE_CARDREMOVESCENE_HPP

#include "../core/Scene.hpp"
#include "../core/SceneTransition.hpp"
#include "../system/EventSystem.hpp"
#include "../system/MapSystem.hpp"

#include <SFML/Graphics.hpp>

#include <string>

class CardRemoveScene : public Scene {
public:
    explicit CardRemoveScene(GameContext& context);

    void handleEvent(
        const sf::Event& event,
        const sf::RenderWindow& window
    ) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) override;

    SceneTransition getTransition() const override;

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

private:
    EventSystem eventSystem_;
    MapSystem mapSystem_;

    SceneTransition transition_;

    int page_ = 0;
    std::string message_;
};

#endif // SPIRELIKE_CARDREMOVESCENE_HPP
