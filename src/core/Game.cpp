#include "Game.hpp"
#include <SFML/Window.hpp>

#include "model/CardInstance.hpp"
#include "model/Enemy.hpp"
#include "model/RunState.hpp"
#include "scene/CombatScene.hpp"
#include "scene/EndScene.hpp"
#include "scene/MapScene.hpp"
#include "scene/MenuScene.hpp"
#include "scene/RewardScene.hpp"
#include "scene/CharacterSelectScene.hpp"

#define WINDOWRESOULUTION {1920,1080}
using namespace sf;
using namespace std;


Game::Game()
    : window(
          sf::VideoMode({1920, 1080}),
          "SpireLike"
      ),

      gameContext{
          resources,
          runState,
          cardDatabase,
          enemyDatabase
      }
{
    resources.loadTexture(
    "title",
    "assets/images/title.png"
);

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


void Game::update(float dt)
{
    if (currentScene)
    {
        currentScene->update(dt);
    }

    switchSceneIfNeeded();
}


void Game::render()
{
    window.clear();

    if (currentScene)
    {
        currentScene->draw(window);
    }

    window.display();
}

void Game::switchSceneIfNeeded()
{
    if (!currentScene)
    {
        currentScene =
                std::make_unique<MenuScene>(
                    gameContext
                );
        return;
    }

    SceneTransition transition =
        currentScene->getTransition();

    if (transition.target == SceneType::None)
    {
        return;
    }

    switch (transition.target)
    {
        case SceneType::Menu:
        {
            currentScene =
                std::make_unique<MenuScene>(
                    gameContext
                );
            break;
        }

        case SceneType::Map:
        {
            if (!runState.active)
            {
                startNewRun();
            }

            currentScene =
                std::make_unique<MapScene>(
                    gameContext
                );

            break;
        }

        case SceneType::Combat:
        {
            if (
                !enemyDatabase.exists(
                    transition.enemyId
                )
            )
            {
                currentScene =
                    std::make_unique<MapScene>(
                        gameContext
                    );

                break;
            }

            const EnemyDef& enemyDef =
                enemyDatabase.get(
                    transition.enemyId
                );

            currentScene =
                std::make_unique<CombatScene>(
                    gameContext,
                    enemyDef
                );

            break;
        }

        case SceneType::Reward:
        {
            currentScene =
                std::make_unique<RewardScene>(
                    gameContext
                );

            break;
        }

        case SceneType::End:
        {
            currentScene =
                std::make_unique<EndScene>(
                    gameContext,
                    transition.battleResult
                );

            break;
        }

        case SceneType::CharacterSelect:
        {
            currentScene =
                std::make_unique<CharacterSelectScene>(
                    gameContext
                );
            break;
        }


        default:
            break;
    }
}

void Game::startNewRun()
{
    runState = RunState{};

    runState.active = true;

    runState.player.maxHp = 70;
    runState.player.hp = 70;

    runState.floor = 0;
    runState.gold = 0;

    runState.relics.clear();

    runState.currentEnemyId = 0;

    runState.nextCardInstanceId = 1;

    std::random_device rd;

    runState.rng.seed(
        rd()
    );

    runState.masterDeck.clear();

    for (int i = 0; i < 5; ++i)
    {
        CardInstance card;

        card.instanceId =
            runState.nextCardInstanceId++;

        card.cardId = 1; // Strike

        runState.masterDeck.push_back(card);
    }

    // 5 Defend
    for (int i = 0; i < 5; ++i)
    {
        CardInstance card;

        card.instanceId =
            runState.nextCardInstanceId++;

        card.cardId = 2;

        runState.masterDeck.push_back(card);
    }
}

