#include "ResourceManager.hpp"
#include <map>
ErrorCode ResourceManager::loadTexture(const std::string& id, const std::string& path) {
    if (textureMap.find(id) != textureMap.end()) {
        return ErrorCode::OK;
    }
    Texture texture;
    if (!texture.loadFromFile(path)) {
         return ErrorCode::RESOURCE_LOAD_FAILED;
    }
    textureMap[id] = std::move(texture);
    return ErrorCode::OK;
}

Texture& ResourceManager::getTexture(const std::string& id) {
    auto it = textureMap.find(id);
    if (it == textureMap.end()) {
        throw std::runtime_error("Texture not found: " + id);
    }
    return it->second;
}

bool ResourceManager::hasTexture(const std::string& id) const {
    return textureMap.find(id) != textureMap.end();
}

ErrorCode ResourceManager::loadFont(
    const std::string& id,
    const std::string& path
) {
    if (fontMap.find(id) != fontMap.end()) {
        return ErrorCode::OK;
    }

    sf::Font font;

    if (!font.openFromFile(path)) {
        return ErrorCode::RESOURCE_LOAD_FAILED;
    }

    fontMap[id] = std::move(font);

    return ErrorCode::OK;
}

sf::Font& ResourceManager::getFont(
    const std::string& id
) {
    auto it = fontMap.find(id);

    if (it == fontMap.end()) {
        throw std::runtime_error(
            "Font not found: " + id
        );
    }

    return it->second;
}

bool ResourceManager::hasFont(
    const std::string& id
) const {
    return fontMap.find(id) != fontMap.end();
}


ErrorCode ResourceManager::loadSoundBuffer(
    const std::string& id,
    const std::string& path
) {
    if (soundBufferMap.find(id) != soundBufferMap.end()) {
        return ErrorCode::OK;
    }

    sf::SoundBuffer buffer;

    if (!buffer.loadFromFile(path)) {
        return ErrorCode::RESOURCE_LOAD_FAILED;
    }

    soundBufferMap[id] = std::move(buffer);

    return ErrorCode::OK;
}

sf::SoundBuffer& ResourceManager::getSoundBuffer(
    const std::string& id
) {
    auto it = soundBufferMap.find(id);

    if (it == soundBufferMap.end()) {
        throw std::runtime_error("SoundBuffer not found: " + id);
    }

    return it->second;
}

bool ResourceManager::hasSoundBuffer(
    const std::string& id
) const {
    return soundBufferMap.find(id) != soundBufferMap.end();
}

