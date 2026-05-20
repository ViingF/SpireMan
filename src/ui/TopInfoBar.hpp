#ifndef SPIRELIKE_TOPINFOBAR_HPP
#define SPIRELIKE_TOPINFOBAR_HPP

#include "core/GameContext.hpp"
#include "ui/Button.hpp"

#include <SFML/Graphics.hpp>
#include <optional>

class TopInfoBar {
public:
    static void draw(
    sf::RenderWindow& window,
    GameContext& context,
    const sf::Font& font,
    std::optional<int> hpOverride = std::nullopt,
    bool drawDeckIcon = true,
    bool drawSettingsIcon = true,
    bool drawMapIcon = true
);


    static void layoutDeckButton(
        Button& button,
        const sf::RenderWindow& window
    );

    static void layoutSettingsButton(
        Button& button,
        const sf::RenderWindow& window
    );

    static void layoutMapButton(
    Button& button,
    const sf::RenderWindow& window
);


};

#endif
