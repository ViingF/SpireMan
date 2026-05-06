#include <SFML/Graphics.hpp>

#include "ResourceManager.hpp"
#include "Scene.hpp"
#include "database/CardDatabase.hpp"
#include "database/EnemyDatabase.hpp"
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

private:
    CardDatabase cardDatabase;
    EnemyDatabase enemyDatabase;

    RenderWindow window;
    ResourceManager resources;
    RunState runState;
    std::unique_ptr<Scene> currentScene;
    GameContext gameContext;
};
