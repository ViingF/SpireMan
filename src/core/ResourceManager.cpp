#include "ResourceManager.hpp"
#include <map>

#include "Logger.hpp"

ErrorCode ResourceManager::loadTexture(
    const std::string& id,
    const std::string& path
) {
    if (textureMap.find(id) != textureMap.end()) {
        LOG_DEBUG("Texture already loaded: id=" << id);
        return ErrorCode::OK;
    }

    sf::Texture texture;

    if (!texture.loadFromFile(path)) {
        LOG_ERROR("Texture load failed: id=" << id << ", path=" << path);
        return ErrorCode::RESOURCE_LOAD_FAILED;
    }

    textureMap[id] = std::move(texture);

    LOG_DEBUG("Texture loaded: id=" << id << ", path=" << path);

    return ErrorCode::OK;
}


sf::Texture& ResourceManager::getTexture(const std::string& id) {
    auto it = textureMap.find(id);
    if (it == textureMap.end()) {
        LOG_ERROR("Texture not found: id=" << id);
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
        LOG_ERROR("Font load failed: id=" << id << ", path=" << path);
        return ErrorCode::RESOURCE_LOAD_FAILED;
    }

    fontMap[id] = std::move(font);
    LOG_DEBUG("Font loaded: id=" << id << ", path=" << path);

    return ErrorCode::OK;
}

sf::Font& ResourceManager::getFont(
    const std::string& id
) {
    auto it = fontMap.find(id);

    if (it == fontMap.end()) {
        LOG_ERROR("Font not found: id=" << id);
        throw std::runtime_error("Font not found: " + id);

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
        LOG_ERROR("SoundBuffer load failed: id=" << id << ", path=" << path);
        return ErrorCode::RESOURCE_LOAD_FAILED;
    }

    soundBufferMap[id] = std::move(buffer);

    LOG_DEBUG("SoundBuffer loaded: id=" << id << ", path=" << path);

    return ErrorCode::OK;
}

sf::SoundBuffer& ResourceManager::getSoundBuffer(
    const std::string& id
) {
    auto it = soundBufferMap.find(id);

    if (it == soundBufferMap.end()) {
        LOG_ERROR("SoundBuffer not found: id=" << id);
        throw std::runtime_error("SoundBuffer not found: " + id);

    }

    return it->second;
}

bool ResourceManager::hasSoundBuffer(
    const std::string& id
) const {
    return soundBufferMap.find(id) != soundBufferMap.end();
}

