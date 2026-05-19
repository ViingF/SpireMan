
#include "TextUtils.hpp"


sf::String TextUtils::fromUtf8(const std::string& text)
{
    return sf::String::fromUtf8(text.begin(), text.end());
}

sf::Text TextUtils::createText(
    const sf::Font& font,
    const std::string& text,
    unsigned int characterSize,
    sf::Color color,
    sf::Vector2f position
) {
    sf::Text txt(font);
    txt.setString(text);
    txt.setCharacterSize(characterSize);
    txt.setFillColor(color);
    txt.setPosition(position);
    return txt;
}

sf::Text TextUtils::createWhiteText(
    const sf::Font& font,
    const std::string& text,
    unsigned int characterSize,
    sf::Vector2f position
) {
    return createText(font, text, characterSize, sf::Color::White, position);
}

sf::Text TextUtils::createBlackText(
    const sf::Font& font,
    const std::string& text,
    unsigned int characterSize,
    sf::Vector2f position
) {
    return createText(font, text, characterSize, sf::Color::Black, position);
}

sf::Text TextUtils::createGrayText(
    const sf::Font& font,
    const std::string& text,
    unsigned int characterSize,
    sf::Vector2f position
) {
    return createText(font, text, characterSize, sf::Color(100, 100, 100), position);
}

std::string TextUtils::formatHpText(int nowHp, int maxHp) {
    return std::to_string(nowHp) + "/" + std::to_string(maxHp);
}
