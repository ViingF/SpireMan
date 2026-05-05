#include "ResourceManager.hpp"
#include <map>
map<string,Texture> textureMap;
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

