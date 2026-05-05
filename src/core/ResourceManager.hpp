
#ifndef SPIRELIKE_RESOURCEMANAGER_HPP
#define SPIRELIKE_RESOURCEMANAGER_HPP
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "model/Types.hpp"

using namespace std;
using namespace sf;
class ResourceManager {
public:
    ErrorCode loadTexture(const std::string& id, const std::string& path);
    ErrorCode loadFont(const std::string& id, const std::string& path);
    ErrorCode loadSoundBuffer(const std::string& id, const std::string& path);

    Texture& getTexture(const std::string& id);
    Font& getFont(const std::string& id);
    SoundBuffer& getSoundBuffer(const std::string& id);

    bool hasTexture(const std::string& id) const;
    bool hasFont(const std::string& id) const;
};
#endif //SPIRELIKE_RESOURCEMANAGER_HPP