#pragma once

#include <SFML/Audio.hpp>

#include <map>
#include <string>
#include <vector>

#include "ResourceManager.hpp"
#include "model/Types.hpp"

enum class MusicFadeState {
    Idle,
    FadingOut,
    FadingIn
};

class AudioManager {
public:
    explicit AudioManager(ResourceManager& resources);

    ErrorCode loadMusic(
        const std::string& id,
        const std::string& path
    );

    void playMusic(
        const std::string& id,
        bool loop = true
    );

    void stopMusic();

    void playSound(
        const std::string& id
    );

    void update(float dt);

    void setMusicVolume(float volume);
    void setSoundVolume(float volume);


private:
    void startMusicNow(
        const std::string& id,
        bool loop,
        float startVolume
    );

    MusicFadeState musicFadeState_ = MusicFadeState::Idle;

    std::string pendingMusicId_;
    bool pendingLoop_ = true;

    float fadeDuration_ = 1.8f;
    float fadeTimer_ = 0.0f;
    ResourceManager& resources_;

    std::map<std::string, std::string> musicPaths_;
    std::string currentMusicId_;

    sf::Music music_;
    std::vector<sf::Sound> activeSounds_;

    float musicVolume_ = 35.0f;
    float soundVolume_ = 40.0f;
};
