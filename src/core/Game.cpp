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
#include "scene/CampfireScene.hpp"
#include "scene/CardRemoveScene.hpp"
#include "scene/ShopScene.hpp"

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
    resources.loadTexture("campfire","assets/images/rest.png");
    resources.loadTexture("campfireOutline","assets/images/restOutline.png");
    resources.loadTexture("shop","assets/images/shop.png");
    resources.loadTexture("shopOutline","assets/images/shopOutline.png");

    resources.loadTexture("Kaka","assets/images/kaka.png");
    resources.loadTexture("Rat","assets/images/rat.png");
    resources.loadTexture("Slime","assets/images/slime.png");
    resources.loadTexture("SlimeMap","assets/images/slimemap.png");
    resources.loadTexture("Guardian","assets/images/guardian.png");
    resources.loadTexture("GuardianMap","assets/images/guardianmap.png");
    resources.loadTexture("Automaton","assets/images/Automaton.png");
    resources.loadTexture("Cultist","assets/images/Cultist.png");
    resources.loadTexture("ShelledParasite","assets/images/Shelled Parasite.png");
    resources.loadTexture("SneakyGremlin","assets/images/Sneaky Gremlin.png");
    resources.loadTexture("SphericGuardian","assets/images/Spheric Guardian.png");
    resources.loadTexture("TheCollector","assets/images/The Collector.png");
    resources.loadTexture("AutomatonMap","assets/images/automatonmap.png");
    resources.loadTexture("CollectorMap","assets/images/collectormap.png");
    resources.loadTexture("AwakenedOne","assets/images/Awakened One.png");
    resources.loadTexture("AwakenedOneMap","assets/images/awakenedmap.png");
    resources.loadTexture("SpireGrowth","assets/images/Spire Growth.png");
    resources.loadTexture("TimeEater","assets/images/TimeEater.png");
    resources.loadTexture("TimeEaterMap","assets/images/timeeatermap.png");
    resources.loadTexture("OrbWalker","assets/images/Orb Walker.png");
    resources.loadTexture("Maw","assets/images/Maw.png");
    resources.loadTexture("SpireGrowth","assets/images/Spire Growth.png");
    resources.loadTexture("Darkling","assets/images/Darkling.png");

    resources.loadTexture("Fishing","assets/images/fishing.jpg");
    resources.loadTexture("GoldenWing","assets/images/goldenWing.jpg");
    resources.loadTexture("GoopPuddle","assets/images/goopPuddle.jpg");
    resources.loadTexture("LiarsGame","assets/images/liarsGame.jpg");
    resources.loadTexture("ShiningLight","assets/images/shiningLight.jpg");
    resources.loadTexture("BackToBasics","assets/images/backToBasics.jpg");
    resources.loadTexture("Cleric","assets/images/cleric.jpg");
    resources.loadTexture("CursedTome","assets/images/cursedTome.jpg");
    resources.loadTexture("GoldenIdol","assets/images/goldenIdol.jpg");
    resources.loadTexture("LadyInBlue","assets/images/ladyInBlue.jpg");
    resources.loadTexture("Library","assets/images/library.jpg");
    resources.loadTexture("Mausoleum","assets/images/mausoleum.jpg");
    resources.loadTexture("MoaiHead","assets/images/moaiHead.jpg");
    resources.loadTexture("TheNest","assets/images/theNest.jpg");

    resources.loadTexture("Writhe","assets/images/writhe.png");
    resources.loadTexture("Necronomicurse","assets/images/necronomicurse.png");
    resources.loadTexture("Strike","assets/images/strike.png");
    resources.loadTexture("Defend","assets/images/defend.png");
    resources.loadTexture("Bash","assets/images/bash.png");
    resources.loadTexture("RitualDagger","assets/images/ritual_dagger.png");


    resources.loadFont("zh-B","assets/fonts/NotoSansCJKtc-Bold.otf");
    resources.loadFont("zh-M","assets/fonts/NotoSansCJKtc-Medium.otf");
    resources.loadFont("zh-R","assets/fonts/NotoSansCJKtc-Regular.otf");
    auto eventCode=eventDatabase.loadFromCsv("data/events.csv");
    auto enemyCode=enemyDatabase.loadFromCsv("data/enemies.csv");
    auto cardCode=cardDatabase.loadFromCsv("data/cards.csv");

    if (eventCode !=ErrorCode::OK||enemyCode !=ErrorCode::OK||cardCode !=ErrorCode::OK) {
        throw std::runtime_error(
        "Database load failed. events=" +
        std::to_string(static_cast<int>(eventCode)) +
        ", enemies=" +
        std::to_string(static_cast<int>(enemyCode)) +
        ", cards=" +
        std::to_string(static_cast<int>(cardCode))
    );
    }


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
        case SceneType::Campfire:
            currentScene =
                 std::make_unique<CampfireScene>(
                     gameContext
                 );
            break;

        case SceneType::Shop:
            currentScene =
                 std::make_unique<ShopScene>(
                     gameContext
                 );
            break;
        case SceneType::CardRemove:
        {
            currentScene =
                std::make_unique<CardRemoveScene>(
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
    runState.gold = 99;
    runState.relics.clear();

    runState.currentEnemyId = 0;
    runState.nextCardInstanceId = 1;
    runState.act = 1;
    runState.floor = 0;
    runState.floorInAct = 0;


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
    mapSystem.startAct(
        runState,
        1,
        eventDatabase,
        enemyDatabase
    );

    mapSystem.generateRouteMap(runState, eventDatabase);

    currentScene = std::make_unique<MapScene>(gameContext);

}


