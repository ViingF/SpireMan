#pragma once

#include <SFML/Graphics.hpp>

#include "ResourceManager.hpp"
#include "Scene.hpp"
#include "database/CardDatabase.hpp"
#include "database/EnemyDatabase.hpp"
#include "database/EventDatabase.hpp"
#include "model/RunState.hpp"

using namespace sf;
class Game {
public:
    Game();
    void run();

private:
    void startNewRun();
    void processEvents();
    void update(float dt);
    void render();
    void switchSceneIfNeeded();
    void playMusicForScene(SceneType sceneType);


private:
    CardDatabase cardDatabase;
    EnemyDatabase enemyDatabase;
    EventDatabase eventDatabase;
    EncounterDatabase encounterDatabase;

    sf::RenderWindow window;
    ResourceManager resources;
    AudioManager audio;
    RunState runState;
    std::unique_ptr<Scene> currentScene;
    GameContext gameContext;
    std::unique_ptr<Scene> suspendedScene_;

    SceneType currentSceneType_ = SceneType::None;
    SceneType suspendedSceneType_ = SceneType::None;
    FailureToast failureToast_;
};
