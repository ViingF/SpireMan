
#ifndef SPIRELIKE_EVENTSCENE_HPP
#define SPIRELIKE_EVENTSCENE_HPP

#include "../core/Scene.hpp"
#include "../core/SceneTransition.hpp"
#include "../system/EventSystem.hpp"
#include "../system/MapSystem.hpp"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

#include "ui/Button.hpp"

class EventScene : public Scene {
public:
    EventScene(GameContext& context, EventId eventId);

    void handleEvent(
        const sf::Event& event,
        const sf::RenderWindow& window
    ) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) override;

    SceneTransition getTransition() const override;
    void resetTransition() override;

private:
    sf::FloatRect getChoiceRect(int index) const;

    void choose(int choiceIndex);
    bool canChoose(const EventChoiceDef& choice) const;
    void createChoiceButtons();
    void syncChoiceButtons();
    void drawBackground(sf::RenderWindow& window);

private:
    EventId eventId_ = 0;

    EventSystem eventSystem_;
    MapSystem mapSystem_;

    SceneTransition transition_;

    bool resolved_ = false;
    std::string resultMessage_;
    std::vector<Button> choiceButtons_;
    Button mapIconButton_;


};
#endif //SPIRELIKE_EVENTSCENE_HPP