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
#include "../scene/EventScene.hpp"
#include "../system/MapSystem.hpp"

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
          enemyDatabase,
          eventDatabase
      }
{
    window.setFramerateLimit(60);
    resources.loadTexture("title","assets/images/title.png");
    resources.loadTexture("mapTop","assets/images/mapTop.png");
    resources.loadTexture("mapBot","assets/images/mapBot.png");
    resources.loadTexture("monster","assets/images/monster.png");
    resources.loadTexture("monsterOutline","assets/images/monsterOutline.png");
    resources.loadTexture("event","assets/images/event.png");
    resources.loadTexture("eventOutline","assets/images/eventOutline.png");
    resources.loadTexture("ironcladButton","assets/images/ironcladButton.png");
    resources.loadTexture("ironcladPortrait","assets/images/ironcladPortrait.jpg");
    resources.loadTexture("lockedButton","assets/images/lockedButton.png");
    resources.loadTexture("fishing","assets/images/fishing.jpg");
    resources.loadTexture("goldenWing","assets/images/goldenWing.jpg");
    resources.loadTexture("goopPuddle","assets/images/goopPuddle.jpg");
    resources.loadTexture("liarsGame","assets/images/liarsGame.jpg");
    resources.loadTexture("shiningLight","assets/images/shiningLight.jpg");
    resources.loadTexture("kaka","assets/images/kaka.png");
    resources.loadTexture("rat","assets/images/rat.png");
    resources.loadTexture("slime","assets/images/slime.png");
    resources.loadTexture("slimemap","assets/images/slimemap.png");
    resources.loadTexture("guardian","assets/images/guardian.png");
    resources.loadTexture("guardianmap","assets/images/guardianmap.png");

    resources.loadFont("zh-B","assets/fonts/NotoSansCJKtc-Bold.otf");
    resources.loadFont("zh-M","assets/fonts/NotoSansCJKtc-Medium.otf");
    resources.loadFont("zh-R","assets/fonts/NotoSansCJKtc-Regular.otf");
    eventDatabase.loadFromCsv("data/events.csv");
    enemyDatabase.loadFromCsv("data/enemies.csv");
    cardDatabase.loadFromCsv("data/cards.csv");

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
        case SceneType::Event:
            if (!eventDatabase.exists(transition.eventId)) {
                currentScene = std::make_unique<MapScene>(gameContext);
                break;
            }

            currentScene = std::make_unique<EventScene>(
                gameContext,
                transition.eventId
            );
            break;

        case SceneType::Map:
        {
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
            startNewRun();
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
    runState.rng = std::mt19937(rd());

    runState.masterDeck.clear();

    runState.masterDeck.push_back({runState.nextCardInstanceId++, 1});
    runState.masterDeck.push_back({runState.nextCardInstanceId++, 1});
    runState.masterDeck.push_back({runState.nextCardInstanceId++, 1});
    runState.masterDeck.push_back({runState.nextCardInstanceId++, 1});
    runState.masterDeck.push_back({runState.nextCardInstanceId++, 2});
    runState.masterDeck.push_back({runState.nextCardInstanceId++, 2});
    runState.masterDeck.push_back({runState.nextCardInstanceId++, 2});
    runState.masterDeck.push_back({runState.nextCardInstanceId++, 2});
    runState.masterDeck.push_back({runState.nextCardInstanceId++, 3});

    MapSystem mapSystem;
    mapSystem.generateSingleRoute(runState, eventDatabase);

    GameContext context {
        resources,
        runState,
        cardDatabase,
        enemyDatabase,
        eventDatabase
    };

    currentScene = std::make_unique<MapScene>(context);
}


