#include "AudioManager.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>

#include "Logger.hpp"

AudioManager::AudioManager(ResourceManager& resources)
    : resources_(resources)
{
}

ErrorCode AudioManager::loadMusic(
    const std::string& id,
    const std::string& path
) {
    musicPaths_[id] = path;
    return ErrorCode::OK;
}

void AudioManager::playMusic(
    const std::string& id,
    bool loop
) {
    LOG_INFO("Request play music: id=" << id << ", loop=" << loop);

    if (currentMusicId_ == id && pendingMusicId_.empty()) {
        return;
    }

    if (currentMusicId_.empty()) {
        startMusicNow(id, loop, 0.0f);

        musicFadeState_ = MusicFadeState::FadingIn;
        fadeTimer_ = 0.0f;

        return;
    }

    pendingMusicId_ = id;
    pendingLoop_ = loop;

    if (musicFadeState_ != MusicFadeState::FadingOut) {
        musicFadeState_ = MusicFadeState::FadingOut;
        fadeTimer_ = 0.0f;
    }
}


void AudioManager::stopMusic()
{
    pendingMusicId_.clear();

    if (!currentMusicId_.empty()) {
        musicFadeState_ = MusicFadeState::FadingOut;
        fadeTimer_ = 0.0f;
    }
}


void AudioManager::playSound(const std::string& id)
{
    if (!resources_.hasSoundBuffer(id)) {
        LOG_WARN("SoundBuffer not found: id=" << id);
        return;
    }

    activeSounds_.emplace_back(resources_.getSoundBuffer(id));

    sf::Sound& sound = activeSounds_.back();
    sound.setVolume(soundVolume_);
    sound.play();

}


void AudioManager::update(float dt)
{
    activeSounds_.erase(
        std::remove_if(
            activeSounds_.begin(),
            activeSounds_.end(),
            [](const sf::Sound& sound) {
                return sound.getStatus()
                    == sf::SoundSource::Status::Stopped;
            }
        ),
        activeSounds_.end()
    );

    if (musicFadeState_ == MusicFadeState::Idle) {
        return;
    }

    fadeTimer_ += dt;

    float t = fadeTimer_ / fadeDuration_;

    if (t > 1.0f) {
        t = 1.0f;
    }

    if (musicFadeState_ == MusicFadeState::FadingOut) {
        float volume = musicVolume_ * (1.0f - t);
        music_.setVolume(volume);

        if (t >= 1.0f) {
            music_.stop();
            currentMusicId_.clear();

            if (!pendingMusicId_.empty()) {
                std::string nextId = pendingMusicId_;
                bool nextLoop = pendingLoop_;

                pendingMusicId_.clear();

                startMusicNow(nextId, nextLoop, 0.0f);

                musicFadeState_ = MusicFadeState::FadingIn;
                fadeTimer_ = 0.0f;
            } else {
                musicFadeState_ = MusicFadeState::Idle;
            }
        }
    } else if (musicFadeState_ == MusicFadeState::FadingIn) {
        float volume = musicVolume_ * t;
        music_.setVolume(volume);

        if (t >= 1.0f) {
            music_.setVolume(musicVolume_);
            musicFadeState_ = MusicFadeState::Idle;
        }
    }
}


void AudioManager::setMusicVolume(float volume)
{
    musicVolume_ = volume;

    if (musicFadeState_ == MusicFadeState::Idle) {
        music_.setVolume(musicVolume_);
    }
}


void AudioManager::setSoundVolume(float volume)
{
    soundVolume_ = volume;
}

void AudioManager::startMusicNow(
    const std::string& id,
    bool loop,
    float startVolume
) {
    auto it = musicPaths_.find(id);

    if (it == musicPaths_.end()) {
        LOG_ERROR("Music id not found: id=" << id);
        throw std::runtime_error("Music not found: " + id);
    }

    music_.stop();

    if (!music_.openFromFile(it->second)) {
        LOG_ERROR("Music load failed: id=" << id << ", path=" << it->second);
        throw std::runtime_error("Music load failed: " + it->second);
    }

    LOG_INFO("Music started: id=" << id << ", path=" << it->second);

    music_.setVolume(startVolume);
    music_.setLooping(loop);
    music_.play();

    currentMusicId_ = id;
}

