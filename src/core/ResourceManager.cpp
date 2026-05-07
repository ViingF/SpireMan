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

