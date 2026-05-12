cmake_minimum_required(VERSION 4.1)
project(SpireLike)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR "D:/SFML-3.0.2/lib/cmake/SFML")

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(SpireLike src/main.cpp
        src/core/Scene.hpp
        src/core/Game.hpp
        src/core/Game.cpp
        src/core/ResourceManager.hpp
        src/core/ResourceManager.cpp
        src/scene/MenuScene.cpp
        src/scene/MenuScene.hpp
        src/scene/MapScene.cpp
        src/scene/MapScene.hpp
        src/scene/EndScene.hpp
        src/scene/EndScene.cpp
        src/core/SceneTransition.hpp
        src/core/GameContext.hpp
        src/core/Logger.hpp
        src/core/Logger.cpp
        src/database/DataLoader.cpp
        src/database/DataLoader.hpp
        src/database/CardDatabase.cpp
        src/database/CardDatabase.hpp
        src/database/EnemyDatabase.cpp
        src/database/EnemyDatabase.hpp
        src/database/RelicDatabase.cpp
        src/database/RelicDatabase.hpp
        src/config/Constants.hpp
        src/config/Paths.hpp
        src/model/Types.hpp
        src/system/CombatSystem.cpp
        src/system/CombatSystem.hpp
        src/ui/Button.cpp
        src/ui/Button.hpp
        src/scene/RewardScene.cpp
        src/scene/RewardScene.hpp
        src/scene/CombatScene.cpp
        src/scene/CombatScene.hpp
        src/model/CardDef.hpp
        src/model/CardInstance.hpp
        src/model/RunState.hpp
        src/model/CombatDeck.hpp
        src/model/Player.hpp
        src/model/Enemy.hpp
        src/scene/CharacterSelectScene.cpp
        src/scene/CharacterSelectScene.hpp
        src/model/EventDef.hpp
        src/model/MapNode.hpp
        src/database/EventDatabase.cpp
        src/database/EventDatabase.hpp
        src/system/MapSystem.cpp
        src/system/MapSystem.hpp
        src/system/EventSystem.cpp
        src/system/EventSystem.hpp
        src/scene/EventScene.cpp
        src/scene/EventScene.hpp
        src/scene/CampfireScene.cpp
        src/scene/CampfireScene.hpp
        src/scene/ShopScene.cpp
        src/scene/ShopScene.hpp
        src/scene/CardRemoveScene.cpp
        src/scene/CardRemoveScene.hpp)

target_link_libraries(SpireLike
        SFML::Graphics
        SFML::Window
        SFML::System
)

target_include_directories(SpireLike PRIVATE src)
