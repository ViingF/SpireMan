#include "Game.hpp"
#include <SFML/Window.hpp>
#define WINDOWRESOULUTION {1920,1080}
using namespace sf;
using namespace std;


Game::Game() {
    window.create(VideoMode(WINDOWRESOULUTION),"SFML Window");
    resources.loadTexture("title","assets/images/title.png");
}

void Game::processEvents()
{
    while (const std::optional<sf::Event> event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
            return;
        }
        if (currentScene)
        {
            currentScene->handleEvent(*event, window);
        }
    }
}

void Game::run()
{
    Clock clock;
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        processEvents();
        update(dt);
        render();
    }
}


void Game::update(float dt) {
    RectangleShape rect;
    rect.setSize(sf::Vector2f(WINDOWRESOULUTION));
    rect.setTexture(&resources.getTexture("title"));
    window.draw(rect);
}

void Game::render() {
    window.clear();
    update(1);
    window.display();
}
