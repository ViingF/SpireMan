
#ifndef SPIRELIKE_TEXTUTILS_HPP
#define SPIRELIKE_TEXTUTILS_HPP

#include <SFML/Graphics.hpp>
#include <string>

class TextUtils {
public:
    static sf::Text createText(
        const sf::Font& font,
        const std::string& text,
        unsigned int characterSize,
        sf::Color color,
        sf::Vector2f position
    );

    static sf::String fromUtf8(const std::string& text);

    static sf::Text createWhiteText(
        const sf::Font& font,
        const std::string& text,
        unsigned int characterSize,
        sf::Vector2f position
    );

    static sf::Text createBlackText(
        const sf::Font& font,
        const std::string& text,
        unsigned int characterSize,
        sf::Vector2f position
    );

    static sf::Text createGrayText(
        const sf::Font& font,
        const std::string& text,
        unsigned int characterSize,
        sf::Vector2f position
    );

    static std::string formatHpText(int nowHp, int maxHp);
};


#endif //SPIRELIKE_TEXTUTILS_HPP