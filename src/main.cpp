#include <SFML/Graphics.hpp>
#include <iostream>

#include "core/Game.hpp"
#include "core/Logger.hpp"
#include "config/Paths.hpp"

#define WINDOWRESOULUTION {1920,1080}

using namespace sf;
using namespace std;

int main() {
    Logger::instance().init(
        FILEPATH + "/logs/game.log",
        LogLevel::Debug,
        true
    );

    LOG_INFO("Program started");

    Game game;
    game.run();

    LOG_INFO("Program ended");
    Logger::instance().shutdown();

    return 0;
}
