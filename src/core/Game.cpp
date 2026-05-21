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
#include "system/MapSystem.hpp"
#include "system/SaveSystem.hpp"

using namespace sf;
using namespace std;


Game::Game()
    : window(
          sf::VideoMode({1920, 1080}),
          "SpireLike"
      ),
      audio(resources),
      gameContext{
          resources,
          runState,
          cardDatabase,
          enemyDatabase,
          eventDatabase,
          encounterDatabase,
          audio
      }


{
    window.setFramerateLimit(60);

    //其他
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
    resources.loadTexture("Sleep","assets/images/sleep.png");
    resources.loadTexture("Campfire","assets/images/campfire.png");
    resources.loadTexture("hp","assets/images/hp.png");
    resources.loadTexture("energy","assets/images/energyRedVFX.png");
    resources.loadTexture("blank","assets/images/blank.png");
    resources.loadTexture("IronClad","assets/images/IronClad.png");
    resources.loadTexture("block","assets/images/block.png");
    resources.loadTexture("burningBlood","assets/images/burningBlood.png");
    resources.loadTexture("deck","assets/images/deck.png");
    resources.loadTexture("settings","assets/images/settings.png");
    resources.loadTexture("panelHeart","assets/images/panelHeart.png");
    resources.loadTexture("strength","assets/images/strength.png");
    resources.loadTexture("weak","assets/images/weak.png");
    resources.loadTexture("vulnerable","assets/images/vulnerable.png");
    resources.loadTexture("floor","assets/images/floor.png");
    resources.loadTexture("background","assets/images/background.png");
    resources.loadTexture("panelGoldBag","assets/images/panelGoldBag.png");
    resources.loadTexture("map","assets/images/map.png");
    resources.loadTexture("drawPile","assets/images/drawPile.png");
    resources.loadTexture("discardPile","assets/images/discardPile.png");
    resources.loadTexture("endTurnButton","assets/images/endTurnButton.png");
    resources.loadTexture("attack","assets/images/attack.png");
    resources.loadTexture("buff","assets/images/buff1.png");
    resources.loadTexture("exhaustPile","assets/images/ascension.png");
    resources.loadTexture("shopbackground","assets/images/shopbackground.png");
    resources.loadTexture("eventbackground","assets/images/eventbackground.png");
    resources.loadTexture("enabledButton","assets/images/enabledButton.png");
    resources.loadTexture("cancelButton","assets/images/cancelButton.png");
    resources.loadTexture("removecard","assets/images/removecard.png");
    resources.loadTexture("confirmButton","assets/images/confirmButton.png");
    resources.loadTexture("death","assets/images/death.png");


    //怪物
    resources.loadTexture("AcidSlimeL","assets/images/enemies/AcidSlimeL.png");
    resources.loadTexture("AcidSlimeM","assets/images/enemies/AcidSlimeM.png");
    resources.loadTexture("AcidSlimeS","assets/images/enemies/AcidSlimeS.png");
    resources.loadTexture("SpikeSlimeL","assets/images/enemies/SpikeSlimeL.png");
    resources.loadTexture("SpikeSlimeM","assets/images/enemies/SpikeSlimeM.png");
    resources.loadTexture("SpikeSlimeS","assets/images/enemies/SpikeSlimeS.png");
    resources.loadTexture("Cultist","assets/images/enemies/Cultist.png");
    resources.loadTexture("JawWorm","assets/images/enemies/JawWorm.png");
    resources.loadTexture("RedLouse","assets/images/enemies/RedLouse.png");
    resources.loadTexture("GreenLouse","assets/images/enemies/GreenLouse.png");
    resources.loadTexture("FungiBeast","assets/images/enemies/FungiBeast.png");
    resources.loadTexture("MadGremlin","assets/images/enemies/MadGremlin.png");
    resources.loadTexture("SneakyGremlin","assets/images/enemies/SneakyGremlin.png");
    resources.loadTexture("FatGremlin","assets/images/enemies/FatGremlin.png");
    resources.loadTexture("GremlinWizard","assets/images/enemies/GremlinWizard.png");
    resources.loadTexture("ShieldGremlin","assets/images/enemies/ShieldGremlin.png");
    resources.loadTexture("Looter","assets/images/enemies/Looter.png");
    resources.loadTexture("BlueSlaver","assets/images/enemies/BlueSlaver.png");
    resources.loadTexture("RedSlaver","assets/images/enemies/RedSlaver.png");
    resources.loadTexture("GremlinNob","assets/images/enemies/GremlinNob.png");
    resources.loadTexture("Lagavulin","assets/images/enemies/Lagavulin.png");
    resources.loadTexture("Sentry","assets/images/enemies/Sentry.png");
    resources.loadTexture("Byrd","assets/images/enemies/Byrd.png");
    resources.loadTexture("Chosen","assets/images/enemies/Chosen.png");
    resources.loadTexture("Mugger","assets/images/enemies/Mugger.png");
    resources.loadTexture("Centurion","assets/images/enemies/Centurion.png");
    resources.loadTexture("Mystic","assets/images/enemies/Mystic.png");
    resources.loadTexture("ShelledParasite","assets/images/enemies/ShelledParasite.png");
    resources.loadTexture("SnakePlant","assets/images/enemies/SnakePlant.png");
    resources.loadTexture("Snecko","assets/images/enemies/Snecko.png");
    resources.loadTexture("SphericGuardian","assets/images/enemies/SphericGuardian.png");
    resources.loadTexture("BookOfStabbing","assets/images/enemies/BookOfStabbing.png");
    resources.loadTexture("GremlinLeader","assets/images/enemies/GremlinLeader.png");
    resources.loadTexture("Taskmaster","assets/images/enemies/Taskmaster.png");
    resources.loadTexture("Bear","assets/images/enemies/Bear.png");
    resources.loadTexture("Romeo","assets/images/enemies/Romeo.png");
    resources.loadTexture("Pointy","assets/images/enemies/Pointy.png");
    resources.loadTexture("Darkling","assets/images/enemies/Darkling.png");
    resources.loadTexture("OrbWalker","assets/images/enemies/OrbWalker.png");
    resources.loadTexture("Repulsor","assets/images/enemies/Repulsor.png");
    resources.loadTexture("Spiker","assets/images/enemies/Spiker.png");
    resources.loadTexture("Exploder","assets/images/enemies/Exploder.png");
    resources.loadTexture("TheMaw","assets/images/enemies/TheMaw.png");
    resources.loadTexture("SpireGrowth","assets/images/enemies/SpireGrowth.png");
    resources.loadTexture("Transient","assets/images/enemies/Transient.png");
    resources.loadTexture("WrithingMass","assets/images/enemies/WrithingMass.png");
    resources.loadTexture("GiantHead","assets/images/enemies/GiantHead.png");
    resources.loadTexture("Nemesis","assets/images/enemies/Nemesis.png");
    resources.loadTexture("Reptomancer","assets/images/enemies/Reptomancer.png");
    resources.loadTexture("Dagger","assets/images/enemies/Dagger.png");
    resources.loadTexture("TorchHead","assets/images/enemies/TorchHead.png");resources.loadTexture("SlimeBoss","assets/images/enemies/SlimeBoss.png");
    resources.loadTexture("TheGuardian","assets/images/enemies/TheGuardian.png");
    resources.loadTexture("BronzeAutomaton","assets/images/enemies/BronzeAutomaton.png");
    resources.loadTexture("TheChamp","assets/images/enemies/TheChamp.png");
    resources.loadTexture("TheCollector","assets/images/enemies/TheCollector.png");
    resources.loadTexture("AwakenedOne","assets/images/enemies/AwakenedOne.png");
    resources.loadTexture("TimeEater","assets/images/enemies/TimeEater.png");
    resources.loadTexture("Donu","assets/images/enemies/Donu.png");
    resources.loadTexture("Deca","assets/images/enemies/Deca.png");

    //boss地图
    resources.loadTexture("SlimeBossMap","assets/images/mapicons/SlimeBossMap.png");
    resources.loadTexture("TheGuardianMap","assets/images/mapicons/TheGuardianMap.png");
    resources.loadTexture("TheChampMap","assets/images/mapicons/TheChampMap.png");
    resources.loadTexture("TheCollectorMap","assets/images/mapicons/TheCollectorMap.png");
    resources.loadTexture("AwakenedOneMap","assets/images/mapicons/AwakenedOneMap.png");
    resources.loadTexture("TimeEaterMap","assets/images/mapicons/TimeEaterMap.png");
    resources.loadTexture("DonuMap","assets/images/mapicons/DonuMap.png");

    //事件图片
    resources.loadTexture("BackToBasics","assets/images/events/backToBasics.jpg");
    resources.loadTexture("Cleric","assets/images/events/cleric.jpg");
    resources.loadTexture("GoldenIdol","assets/images/events/goldenIdol.jpg");
    resources.loadTexture("GoopPuddle","assets/images/events/goopPuddle.jpg");
    resources.loadTexture("LadyInBlue","assets/images/events/ladyInBlue.jpg");
    resources.loadTexture("Library","assets/images/events/library.jpg");
    resources.loadTexture("CursedTome","assets/images/events/cursedTome.jpg");
    resources.loadTexture("MoaiHead","assets/images/events/moaiHead.jpg");
    resources.loadTexture("Mausoleum","assets/images/events/mausoleum.jpg");

    //卡牌
    resources.loadTexture("Attack","assets/images/cards/attack.png");
    resources.loadTexture("Skill","assets/images/cards/skill.png");
    resources.loadTexture("Curse","assets/images/cards/curse.png");
    resources.loadTexture("Strike","assets/images/cards/strike.png");
    resources.loadTexture("Defend","assets/images/cards/defend.png");
    resources.loadTexture("Bash","assets/images/cards/bash.png");
    resources.loadTexture("Clothesline","assets/images/cards/clothesline.png");
    resources.loadTexture("IronWave","assets/images/cards/iron_wave.png");
    resources.loadTexture("PommelStrike","assets/images/cards/pommel_strike.png");
    resources.loadTexture("ShrugItOff","assets/images/cards/shrug_it_off.png");
    resources.loadTexture("TwinStrike","assets/images/cards/twin_strike.png");
    resources.loadTexture("Intimidate","assets/images/cards/intimidate.png");
    resources.loadTexture("Pummel","assets/images/cards/pummel.png");
    resources.loadTexture("SeeingRed","assets/images/cards/seeing_red.png");
    resources.loadTexture("Bludgeon","assets/images/cards/bludgeon.png");
    resources.loadTexture("Impervious","assets/images/cards/impervious.png");
    resources.loadTexture("Necronomicurse","assets/images/cards/necronomicurse.png");
    resources.loadTexture("Writhe","assets/images/cards/writhe.png");

    resources.loadFont("zh-B","assets/fonts/NotoSansCJKtc-Bold.otf");
    resources.loadFont("zh-M","assets/fonts/NotoSansCJKtc-Medium.otf");
    resources.loadFont("zh-R","assets/fonts/NotoSansCJKtc-Regular.otf");

    resources.loadSoundBuffer("Click","assets/audio/sound/click.ogg");

    audio.loadMusic("Menu","assets/audio/music/menu.ogg");
    audio.loadMusic("Act1Map","assets/audio/music/act1map.ogg");
    audio.loadMusic("Act1Combat","assets/audio/music/act1combat.ogg");
    audio.loadMusic("Shop","assets/audio/music/shop.ogg");
    audio.loadMusic("Event","assets/audio/music/event.ogg");
    audio.loadMusic("End","assets/audio/music/end.ogg");
    audio.loadMusic("Campfire","assets/audio/music/campfire.ogg");

    auto eventCode=eventDatabase.loadFromCsv("data/events.csv");
    auto enemyCode=enemyDatabase.loadFromCsv("data/enemies.csv");
    auto cardCode=cardDatabase.loadFromCsv("data/cards.csv");
    auto encounterCode = encounterDatabase.loadFromCsv("data/encounters.csv");


    if (
    eventCode != ErrorCode::OK ||
    enemyCode != ErrorCode::OK ||
    encounterCode != ErrorCode::OK ||
    cardCode != ErrorCode::OK
) {
        throw std::runtime_error(
            "Database load failed. events=" +
            std::to_string(static_cast<int>(eventCode)) +
            ", enemies=" +
            std::to_string(static_cast<int>(enemyCode)) +
            ", encounters=" +
            std::to_string(static_cast<int>(encounterCode)) +
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
    audio.update(dt);

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
        currentSceneType_ = SceneType::Menu;
        playMusicForScene(SceneType::Menu);
        return;
    }

    SceneTransition transition =
    currentScene->getTransition();

    if (transition.closeMapPreview)
    {
        currentScene->resetTransition();

        if (suspendedScene_)
        {
            currentScene = std::move(suspendedScene_);
            currentSceneType_ = suspendedSceneType_;
            suspendedSceneType_ = SceneType::None;

            playMusicForScene(currentSceneType_);
        }

        return;
    }

    if (transition.openMapPreview)
    {
        // 关键修正：必须先清掉原场景的 openMapPreview
        currentScene->resetTransition();

        if (currentSceneType_ == SceneType::Map)
        {
            return;
        }

        suspendedSceneType_ = currentSceneType_;
        suspendedScene_ = std::move(currentScene);

        currentScene =
            std::make_unique<MapScene>(
                gameContext,
                true
            );

        currentSceneType_ = SceneType::Map;

        playMusicForScene(SceneType::Map);
        return;
    }

    if (transition.target == SceneType::None)
    {
        return;
    }



    if (transition.saveAndQuit)
    {
        SaveSystem::saveRun(runState);

        suspendedScene_.reset();
        suspendedSceneType_ = SceneType::None;

        currentScene =
            std::make_unique<MenuScene>(
                gameContext
            );

        currentSceneType_ = SceneType::Menu;

        playMusicForScene(SceneType::Menu);
        return;
    }


    if (transition.loadGame)
    {
        if (SaveSystem::loadRun(runState)) {
            currentScene =
                std::make_unique<MapScene>(
                    gameContext
                );
            currentSceneType_ = SceneType::Map;
            playMusicForScene(SceneType::Map);
        } else {
            currentScene =
                std::make_unique<MenuScene>(
                    gameContext
                );
            currentSceneType_ = SceneType::Menu;
            playMusicForScene(SceneType::Menu);
        }

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
            currentSceneType_ = SceneType::Menu;
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
            currentSceneType_ = SceneType::Event;
            break;

        case SceneType::Map:
        {
            currentScene =
                std::make_unique<MapScene>(
                    gameContext
                );
            currentSceneType_ = SceneType::Map;

            break;
        }

        case SceneType::Combat:
        {
            if (!encounterDatabase.exists(transition.encounterId)) {
                currentScene = std::make_unique<MapScene>(gameContext);
                break;
            }

            const EncounterDef& encounterDef =
                encounterDatabase.get(transition.encounterId);

            currentScene =
                std::make_unique<CombatScene>(
                    gameContext,
                    encounterDef
                );
            currentSceneType_ = SceneType::Combat;


            break;
        }

        case SceneType::Reward:
        {
            currentScene =
                std::make_unique<RewardScene>(
                    gameContext
                );
            currentSceneType_ = SceneType::Reward;

            break;
        }

        case SceneType::End:
        {
            SaveSystem::deleteSave();
            runState.active = false;

            currentScene =
                std::make_unique<EndScene>(
                    gameContext,
                    transition.battleResult
                );
            currentSceneType_ = SceneType::End;

            break;
        }


        case SceneType::CharacterSelect:
        {
            startNewRun();
            currentScene =
                std::make_unique<CharacterSelectScene>(
                    gameContext
                );
            currentSceneType_ = SceneType::CharacterSelect;
            break;
        }
        case SceneType::Campfire:
            currentScene =
                 std::make_unique<CampfireScene>(
                     gameContext
                 );
            currentSceneType_ = SceneType::Campfire;
            break;

        case SceneType::Shop:
            currentScene =
                 std::make_unique<ShopScene>(
                     gameContext
                 );
            currentSceneType_ = SceneType::Shop;
            break;
        case SceneType::CardRemove:
        {
            currentScene =
                std::make_unique<CardRemoveScene>(
                    gameContext
                );
            currentSceneType_ = SceneType::CardRemove;

            break;
        }




        default:
            break;
    }
    currentSceneType_ = transition.target;
    playMusicForScene(transition.target);

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

    runState.currentEncounterId = 0;
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
    encounterDatabase
);


    currentScene = std::make_unique<MapScene>(gameContext);
    currentSceneType_ = SceneType::Map;


}

void Game::playMusicForScene(SceneType sceneType)
{
    switch (sceneType) {
        case SceneType::Menu:
        case SceneType::CharacterSelect:
            audio.playMusic("Menu");
            break;

        case SceneType::Map:
        case SceneType::Reward:
        case SceneType::CardRemove:
            audio.playMusic("Act1Map");
            break;

        case SceneType::Combat:
            audio.playMusic("Act1Combat");
            break;

        case SceneType::Event:
            audio.playMusic("Event");
            break;

        case SceneType::Campfire:
            audio.playMusic("Campfire");
            break;

        case SceneType::Shop:
            audio.playMusic("Shop");
            break;

        case SceneType::End:
            audio.playMusic("End");
            break;

        default:
            break;
    }
}



