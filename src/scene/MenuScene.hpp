
#ifndef SPIRELIKE_MENUSCENE_HPP
#define SPIRELIKE_MENUSCENE_HPP

#include "core/Scene.hpp"
#include "core/GameContext.hpp"
#include "core/SceneTransition.hpp"
#include "ui/Button.hpp"

#include <SFML/Graphics.hpp>

class MenuScene : public Scene {
public:
    explicit MenuScene(GameContext& context);

    void handleEvent(
        const sf::Event& event,
        const sf::RenderWindow& window
    ) override;

    void update(float dt) override;

    void draw(sf::RenderWindow& window) override;

    SceneTransition getTransition() const override;
    void resetTransition() override;

    void handleNameInputEvent(const sf::Event& event);
    void confirmNameInput();
    void executeCommand(const std::string& input);

private:
    Button startButton;
    Button quitButton;
    Button loadButton;
    Button nameButton;

    SceneTransition transition;
    Sprite background;
    bool editingName = false;
    std::string nameInput = "Player";
    std::string menuMessage;

};

#endif //SPIRELIKE_MENUSCENE_HPP