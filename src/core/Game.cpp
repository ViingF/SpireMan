#include "Game.hpp"
#include <SFML/Window.hpp>

#include "Logger.hpp"
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
#include "config/Paths.hpp"
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
    audio,
    failureToast_
}



{
    LOG_INFO("Game initializing");
    window.setFramerateLimit(60);

    //其他
    resources.loadTexture("title",FILEPATH+"/assets/images/title.png");
    resources.loadTexture("mapTop",FILEPATH+"/assets/images/mapTop.png");
    resources.loadTexture("mapBot",FILEPATH+"/assets/images/mapBot.png");
    resources.loadTexture("monster",FILEPATH+"/assets/images/monster.png");
    resources.loadTexture("monsterOutline",FILEPATH+"/assets/images/monsterOutline.png");
    resources.loadTexture("event",FILEPATH+"/assets/images/event.png");
    resources.loadTexture("eventOutline",FILEPATH+"/assets/images/eventOutline.png");
    resources.loadTexture("ironcladButton",FILEPATH+"/assets/images/ironcladButton.png");
    resources.loadTexture("ironcladPortrait",FILEPATH+"/assets/images/ironcladPortrait.jpg");
    resources.loadTexture("lockedButton",FILEPATH+"/assets/images/lockedButton.png");
    resources.loadTexture("campfire",FILEPATH+"/assets/images/rest.png");
    resources.loadTexture("campfireOutline",FILEPATH+"/assets/images/restOutline.png");
    resources.loadTexture("shop",FILEPATH+"/assets/images/shop.png");
    resources.loadTexture("shopOutline",FILEPATH+"/assets/images/shopOutline.png");
    resources.loadTexture("Sleep",FILEPATH+"/assets/images/sleep.png");
    resources.loadTexture("Campfire",FILEPATH+"/assets/images/campfire.png");
    resources.loadTexture("hp",FILEPATH+"/assets/images/hp.png");
    resources.loadTexture("energy",FILEPATH+"/assets/images/energyRedVFX.png");
    resources.loadTexture("blank",FILEPATH+"/assets/images/blank.png");
    resources.loadTexture("IronClad",FILEPATH+"/assets/images/IronClad.png");
    resources.loadTexture("block",FILEPATH+"/assets/images/block.png");
    resources.loadTexture("burningBlood",FILEPATH+"/assets/images/burningBlood.png");
    resources.loadTexture("deck",FILEPATH+"/assets/images/deck.png");
    resources.loadTexture("settings",FILEPATH+"/assets/images/settings.png");
    resources.loadTexture("panelHeart",FILEPATH+"/assets/images/panelHeart.png");
    resources.loadTexture("strength",FILEPATH+"/assets/images/strength.png");
    resources.loadTexture("weak",FILEPATH+"/assets/images/weak.png");
    resources.loadTexture("vulnerable",FILEPATH+"/assets/images/vulnerable.png");
    resources.loadTexture("floor",FILEPATH+"/assets/images/floor.png");
    resources.loadTexture("background",FILEPATH+"/assets/images/background.png");
    resources.loadTexture("panelGoldBag",FILEPATH+"/assets/images/panelGoldBag.png");
    resources.loadTexture("map",FILEPATH+"/assets/images/map.png");
    resources.loadTexture("drawPile",FILEPATH+"/assets/images/drawPile.png");
    resources.loadTexture("discardPile",FILEPATH+"/assets/images/discardPile.png");
    resources.loadTexture("endTurnButton",FILEPATH+"/assets/images/endTurnButton.png");
    resources.loadTexture("attack",FILEPATH+"/assets/images/attack.png");
    resources.loadTexture("buff",FILEPATH+"/assets/images/buff1.png");
    resources.loadTexture("exhaustPile",FILEPATH+"/assets/images/ascension.png");
    resources.loadTexture("shopbackground",FILEPATH+"/assets/images/shopbackground.png");
    resources.loadTexture("eventbackground",FILEPATH+"/assets/images/eventbackground.png");
    resources.loadTexture("enabledButton",FILEPATH+"/assets/images/enabledButton.png");
    resources.loadTexture("cancelButton",FILEPATH+"/assets/images/cancelButton.png");
    resources.loadTexture("removecard",FILEPATH+"/assets/images/removecard.png");
    resources.loadTexture("confirmButton",FILEPATH+"/assets/images/confirmButton.png");
    resources.loadTexture("death",FILEPATH+"/assets/images/death.png");
    for (int i=1;i<6;i++) {
        resources.loadTexture("end"+std::to_string(i),FILEPATH+"/assets/images/end"+std::to_string(i)+".png");
    }
    resources.loadTexture("selectBanner",FILEPATH+"/assets/images/selectBanner.png");

    //怪物
    resources.loadTexture("AcidSlimeL",FILEPATH+"/assets/images/enemies/AcidSlimeL.png");
    resources.loadTexture("AcidSlimeM",FILEPATH+"/assets/images/enemies/AcidSlimeM.png");
    resources.loadTexture("AcidSlimeS",FILEPATH+"/assets/images/enemies/AcidSlimeS.png");
    resources.loadTexture("SpikeSlimeL",FILEPATH+"/assets/images/enemies/SpikeSlimeL.png");
    resources.loadTexture("SpikeSlimeM",FILEPATH+"/assets/images/enemies/SpikeSlimeM.png");
    resources.loadTexture("SpikeSlimeS",FILEPATH+"/assets/images/enemies/SpikeSlimeS.png");
    resources.loadTexture("Cultist",FILEPATH+"/assets/images/enemies/Cultist.png");
    resources.loadTexture("JawWorm",FILEPATH+"/assets/images/enemies/JawWorm.png");
    resources.loadTexture("RedLouse",FILEPATH+"/assets/images/enemies/RedLouse.png");
    resources.loadTexture("GreenLouse",FILEPATH+"/assets/images/enemies/GreenLouse.png");
    resources.loadTexture("FungiBeast",FILEPATH+"/assets/images/enemies/FungiBeast.png");
    resources.loadTexture("MadGremlin",FILEPATH+"/assets/images/enemies/MadGremlin.png");
    resources.loadTexture("SneakyGremlin",FILEPATH+"/assets/images/enemies/SneakyGremlin.png");
    resources.loadTexture("FatGremlin",FILEPATH+"/assets/images/enemies/FatGremlin.png");
    resources.loadTexture("GremlinWizard",FILEPATH+"/assets/images/enemies/GremlinWizard.png");
    resources.loadTexture("ShieldGremlin",FILEPATH+"/assets/images/enemies/ShieldGremlin.png");
    resources.loadTexture("Looter",FILEPATH+"/assets/images/enemies/Looter.png");
    resources.loadTexture("BlueSlaver",FILEPATH+"/assets/images/enemies/BlueSlaver.png");
    resources.loadTexture("RedSlaver",FILEPATH+"/assets/images/enemies/RedSlaver.png");
    resources.loadTexture("GremlinNob",FILEPATH+"/assets/images/enemies/GremlinNob.png");
    resources.loadTexture("Lagavulin",FILEPATH+"/assets/images/enemies/Lagavulin.png");
    resources.loadTexture("Sentry",FILEPATH+"/assets/images/enemies/Sentry.png");
    resources.loadTexture("Byrd",FILEPATH+"/assets/images/enemies/Byrd.png");
    resources.loadTexture("Chosen",FILEPATH+"/assets/images/enemies/Chosen.png");
    resources.loadTexture("Mugger",FILEPATH+"/assets/images/enemies/Mugger.png");
    resources.loadTexture("Centurion",FILEPATH+"/assets/images/enemies/Centurion.png");
    resources.loadTexture("Mystic",FILEPATH+"/assets/images/enemies/Mystic.png");
    resources.loadTexture("ShelledParasite",FILEPATH+"/assets/images/enemies/ShelledParasite.png");
    resources.loadTexture("SnakePlant",FILEPATH+"/assets/images/enemies/SnakePlant.png");
    resources.loadTexture("Snecko",FILEPATH+"/assets/images/enemies/Snecko.png");
    resources.loadTexture("SphericGuardian",FILEPATH+"/assets/images/enemies/SphericGuardian.png");
    resources.loadTexture("BookOfStabbing",FILEPATH+"/assets/images/enemies/BookOfStabbing.png");
    resources.loadTexture("GremlinLeader",FILEPATH+"/assets/images/enemies/GremlinLeader.png");
    resources.loadTexture("Taskmaster",FILEPATH+"/assets/images/enemies/Taskmaster.png");
    resources.loadTexture("Bear",FILEPATH+"/assets/images/enemies/Bear.png");
    resources.loadTexture("Romeo",FILEPATH+"/assets/images/enemies/Romeo.png");
    resources.loadTexture("Pointy",FILEPATH+"/assets/images/enemies/Pointy.png");
    resources.loadTexture("Darkling",FILEPATH+"/assets/images/enemies/Darkling.png");
    resources.loadTexture("OrbWalker",FILEPATH+"/assets/images/enemies/OrbWalker.png");
    resources.loadTexture("Repulsor",FILEPATH+"/assets/images/enemies/Repulsor.png");
    resources.loadTexture("Spiker",FILEPATH+"/assets/images/enemies/Spiker.png");
    resources.loadTexture("Exploder",FILEPATH+"/assets/images/enemies/Exploder.png");
    resources.loadTexture("TheMaw",FILEPATH+"/assets/images/enemies/TheMaw.png");
    resources.loadTexture("SpireGrowth",FILEPATH+"/assets/images/enemies/SpireGrowth.png");
    resources.loadTexture("Transient",FILEPATH+"/assets/images/enemies/Transient.png");
    resources.loadTexture("WrithingMass",FILEPATH+"/assets/images/enemies/WrithingMass.png");
    resources.loadTexture("GiantHead",FILEPATH+"/assets/images/enemies/GiantHead.png");
    resources.loadTexture("Nemesis",FILEPATH+"/assets/images/enemies/Nemesis.png");
    resources.loadTexture("Reptomancer",FILEPATH+"/assets/images/enemies/Reptomancer.png");
    resources.loadTexture("Dagger",FILEPATH+"/assets/images/enemies/Dagger.png");
    resources.loadTexture("TorchHead",FILEPATH+"/assets/images/enemies/TorchHead.png");resources.loadTexture("SlimeBoss",FILEPATH+"/assets/images/enemies/SlimeBoss.png");
    resources.loadTexture("TheGuardian",FILEPATH+"/assets/images/enemies/TheGuardian.png");
    resources.loadTexture("BronzeAutomaton",FILEPATH+"/assets/images/enemies/BronzeAutomaton.png");
    resources.loadTexture("TheChamp",FILEPATH+"/assets/images/enemies/TheChamp.png");
    resources.loadTexture("TheCollector",FILEPATH+"/assets/images/enemies/TheCollector.png");
    resources.loadTexture("AwakenedOne",FILEPATH+"/assets/images/enemies/AwakenedOne.png");
    resources.loadTexture("TimeEater",FILEPATH+"/assets/images/enemies/TimeEater.png");
    resources.loadTexture("Donu",FILEPATH+"/assets/images/enemies/Donu.png");
    resources.loadTexture("Deca",FILEPATH+"/assets/images/enemies/Deca.png");

    //boss地图
    resources.loadTexture("SlimeBossMap",FILEPATH+"/assets/images/mapicons/SlimeBossMap.png");
    resources.loadTexture("TheGuardianMap",FILEPATH+"/assets/images/mapicons/TheGuardianMap.png");
    resources.loadTexture("TheChampMap",FILEPATH+"/assets/images/mapicons/TheChampMap.png");
    resources.loadTexture("TheCollectorMap",FILEPATH+"/assets/images/mapicons/TheCollectorMap.png");
    resources.loadTexture("AwakenedOneMap",FILEPATH+"/assets/images/mapicons/AwakenedOneMap.png");
    resources.loadTexture("TimeEaterMap",FILEPATH+"/assets/images/mapicons/TimeEaterMap.png");
    resources.loadTexture("DonuMap",FILEPATH+"/assets/images/mapicons/DonuMap.png");

    //事件图片
    resources.loadTexture("BackToBasics",FILEPATH+"/assets/images/events/backToBasics.jpg");
    resources.loadTexture("Cleric",FILEPATH+"/assets/images/events/cleric.jpg");
    resources.loadTexture("GoldenIdol",FILEPATH+"/assets/images/events/goldenIdol.jpg");
    resources.loadTexture("GoopPuddle",FILEPATH+"/assets/images/events/goopPuddle.jpg");
    resources.loadTexture("LadyInBlue",FILEPATH+"/assets/images/events/ladyInBlue.jpg");
    resources.loadTexture("Library",FILEPATH+"/assets/images/events/library.jpg");
    resources.loadTexture("CursedTome",FILEPATH+"/assets/images/events/cursedTome.jpg");
    resources.loadTexture("MoaiHead",FILEPATH+"/assets/images/events/moaiHead.jpg");
    resources.loadTexture("Mausoleum",FILEPATH+"/assets/images/events/mausoleum.jpg");

    //卡牌
    resources.loadTexture("Attack",FILEPATH+"/assets/images/cards/attack.png");
    resources.loadTexture("Skill",FILEPATH+"/assets/images/cards/skill.png");
    resources.loadTexture("Curse",FILEPATH+"/assets/images/cards/curse.png");
    resources.loadTexture("Strike",FILEPATH+"/assets/images/cards/strike.png");
    resources.loadTexture("Defend",FILEPATH+"/assets/images/cards/defend.png");
    resources.loadTexture("Bash",FILEPATH+"/assets/images/cards/bash.png");
    resources.loadTexture("Clothesline",FILEPATH+"/assets/images/cards/clothesline.png");
    resources.loadTexture("IronWave",FILEPATH+"/assets/images/cards/iron_wave.png");
    resources.loadTexture("PommelStrike",FILEPATH+"/assets/images/cards/pommel_strike.png");
    resources.loadTexture("ShrugItOff",FILEPATH+"/assets/images/cards/shrug_it_off.png");
    resources.loadTexture("TwinStrike",FILEPATH+"/assets/images/cards/twin_strike.png");
    resources.loadTexture("Intimidate",FILEPATH+"/assets/images/cards/intimidate.png");
    resources.loadTexture("Pummel",FILEPATH+"/assets/images/cards/pummel.png");
    resources.loadTexture("SeeingRed",FILEPATH+"/assets/images/cards/seeing_red.png");
    resources.loadTexture("Bludgeon",FILEPATH+"/assets/images/cards/bludgeon.png");
    resources.loadTexture("Impervious",FILEPATH+"/assets/images/cards/impervious.png");
    resources.loadTexture("Necronomicurse",FILEPATH+"/assets/images/cards/necronomicurse.png");
    resources.loadTexture("Writhe",FILEPATH+"/assets/images/cards/writhe.png");
    resources.loadTexture("Cleave",FILEPATH+"/assets/images/cards/cleave.png");
    resources.loadTexture("Thunderclap",FILEPATH+"/assets/images/cards/thunder_clap.png");
    resources.loadTexture("Shockwave",FILEPATH+"/assets/images/cards/shockwave.png");

    resources.loadFont("zh-B",FILEPATH+"/assets/fonts/NotoSansCJKtc-Bold.otf");
    resources.loadFont("zh-M",FILEPATH+"/assets/fonts/NotoSansCJKtc-Medium.otf");
    resources.loadFont("zh-R",FILEPATH+"/assets/fonts/NotoSansCJKtc-Regular.otf");

    resources.loadSoundBuffer("Click",FILEPATH+"/assets/audio/sound/click.ogg");

    Button::setClickSoundCallback([this]() {
        audio.playSound("Click");
    });


    audio.loadMusic("Menu",FILEPATH+"/assets/audio/music/menu.ogg");
    audio.loadMusic("Act1Map",FILEPATH+"/assets/audio/music/act1map.ogg");
    audio.loadMusic("Act1Combat",FILEPATH+"/assets/audio/music/act1combat.ogg");
    audio.loadMusic("Shop",FILEPATH+"/assets/audio/music/shop.ogg");
    audio.loadMusic("Event",FILEPATH+"/assets/audio/music/event.ogg");
    audio.loadMusic("End",FILEPATH+"/assets/audio/music/end.ogg");
    audio.loadMusic("Campfire",FILEPATH+"/assets/audio/music/campfire.ogg");

    auto eventCode=eventDatabase.loadFromCsv(FILEPATH+"/data/events.csv");
    auto enemyCode=enemyDatabase.loadFromCsv(FILEPATH+"/data/enemies.csv");
    auto cardCode=cardDatabase.loadFromCsv(FILEPATH+"/data/cards.csv");
    auto encounterCode = encounterDatabase.loadFromCsv(FILEPATH+"/data/encounters.csv");

    LOG_INFO(
        "Database load result: events=" << toString(eventCode)
        << ", enemies=" << toString(enemyCode)
        << ", encounters=" << toString(encounterCode)
        << ", cards=" << toString(cardCode)
    );

    if (
    eventCode != ErrorCode::OK ||
    enemyCode != ErrorCode::OK ||
    encounterCode != ErrorCode::OK ||
    cardCode != ErrorCode::OK
) {
        LOG_FATAL("Database load failed");
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


    LOG_INFO("Game initialized successfully");

}


void Game::processEvents()
{
    while (const std::optional<sf::Event> event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            LOG_INFO("Window close requested");
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
    LOG_INFO("Game loop started");

    Clock clock;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        processEvents();
        update(dt);
        render();
    }

    LOG_INFO("Game loop stopped");
}



void Game::update(float dt)
{
    audio.update(dt);

    if (currentScene)
    {
        currentScene->update(dt);
    }

    failureToast_.update(dt);

    switchSceneIfNeeded();
}




void Game::render()
{
    window.clear();

    if (currentScene)
    {
        currentScene->draw(window);
    }

    failureToast_.draw(
        window,
        resources.getFont("zh-R")
    );

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

    LOG_INFO(
        "Scene transition requested: from="
        << toString(currentSceneType_)
        << ", to="
        << toString(transition.target)
        << ", encounterId="
        << transition.encounterId
        << ", eventId="
        << transition.eventId
    );

    if (transition.saveAndQuit)
    {
        LOG_INFO("Save and quit requested");

        if (!SaveSystem::saveRun(runState)) {
            LOG_ERROR("Save failed during save and quit");
        }

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
        LOG_INFO("Load game requested");
        if (SaveSystem::loadRun(runState)) {
            LOG_INFO("Load game success");
            currentScene =
                std::make_unique<MapScene>(
                    gameContext
                );
            currentSceneType_ = SceneType::Map;
            playMusicForScene(SceneType::Map);
        } else {
            LOG_INFO("Load game failed");
            currentScene =
                std::make_unique<MenuScene>(
                    gameContext
                );
            currentSceneType_ = SceneType::Menu;
            playMusicForScene(SceneType::Menu);
            failureToast_.show("读取存档失败");
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
    LOG_INFO("Starting new run");
    const std::string keepPlayerName =
        runState.playerName.empty()
            ? "Player"
            : runState.playerName;

    runState = RunState{};
    runState.playerName = keepPlayerName;

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

    // 1 = 打击 / Strike，2 = 防御 / Defend，3 = 痛击 / Bash
    for (int i = 0; i < 5; ++i) {
        runState.masterDeck.push_back({runState.nextCardInstanceId++, 1});
    }

    for (int i = 0; i < 4; ++i) {
        runState.masterDeck.push_back({runState.nextCardInstanceId++, 2});
    }

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
    LOG_INFO("Play music for scene: " << toString(sceneType));
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



