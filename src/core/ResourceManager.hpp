
#ifndef SPIRELIKE_RESOURCEMANAGER_HPP
#define SPIRELIKE_RESOURCEMANAGER_HPP
#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "model/Types.hpp"


class ResourceManager {
public:
    ErrorCode loadTexture(const std::string& id, const std::string& path);
    ErrorCode loadFont(const std::string& id, const std::string& path);
    ErrorCode loadSoundBuffer(const std::string& id, const std::string& path);

    sf::Texture& getTexture(const std::string& id);

    sf::Font& getFont(const std::string& id);

    sf::SoundBuffer& getSoundBuffer(const std::string& id);
    bool hasTexture(const std::string& id) const;
    bool hasFont(const std::string& id) const;
    bool hasSoundBuffer(const std::string& id) const;

private:
    std::map<std::string, sf::Texture> textureMap;
    std::map<std::string, sf::Font> fontMap;
    std::map<std::string, sf::SoundBuffer> soundBufferMap;

};
#endif //SPIRELIKE_RESOURCEMANAGER_HPP