#include "SoundManager.h"
#include "Resourcemanager/ResourceManager.h"
#include <iostream>
#include <algorithm>

SoundManager &SoundManager::getInstance()
{
    static SoundManager instance;
    return instance;
}

SoundManager::SoundManager()
    : rng_(std::random_device{}()), bgmVolume_(100.0f), sfxVolume_(100.0f), randomMode_(false)
{
    std::cout << "SoundManager initialized" << std::endl;
}

// ========== BGM 控制 ==========

void SoundManager::setBGMPlaylist(const std::vector<std::string> &tracks)
{
    bgmPlaylist_ = tracks;
}

void SoundManager::playRandomBGM()
{
    if (bgmPlaylist_.empty())
    {
        std::cout << "Warning: BGM playlist is empty" << std::endl;
        return;
    }

    std::uniform_int_distribution<> dis(0, bgmPlaylist_.size() - 1);
    int index = dis(rng_);
    randomMode_ = true;
    playBGM(bgmPlaylist_[index], false);
}

void SoundManager::playBGM(const std::string &path, bool loop)
{
    if (currentBGM_ && currentBGM_->getStatus() == sf::Music::Status::Playing)
    {
        currentBGM_->stop();
    }

    currentBGM_ = std::make_unique<sf::Music>();
    if (currentBGM_->openFromFile(path))
    {
        currentBGM_->setLooping(loop);
        currentBGM_->setVolume(bgmVolume_);
        currentBGM_->play();
        std::cout << "Playing BGM: " << path << std::endl;
    }
    else
    {
        std::cout << "Failed to load BGM: " << path << std::endl;
    }
}

void SoundManager::stopBGM()
{
    if (currentBGM_)
    {
        currentBGM_->stop();
    }
}

void SoundManager::pauseBGM()
{
    if (currentBGM_ && currentBGM_->getStatus() == sf::Music::Status::Playing)
    {
        currentBGM_->pause();
    }
}

void SoundManager::resumeBGM()
{
    if (currentBGM_ && currentBGM_->getStatus() == sf::Music::Status::Paused)
    {
        currentBGM_->play();
    }
}

void SoundManager::setBGMVolume(float volume)
{
    bgmVolume_ = std::max(0.0f, std::min(100.0f, volume));
    if (currentBGM_)
    {
        currentBGM_->setVolume(bgmVolume_);
    }
}

// ========== 音效控制 ==========

void SoundManager::playSFX(const std::string &id)
{
    if (!ResourceManager::GetInstance().HasSoundBuffer(id))
    {
        std::cout << "Warning: Sound buffer not found: " << id << std::endl;
        return;
    }

    // SFML 3 中 sf::Sound 需要传入 SoundBuffer 构造
    auto sound = std::make_unique<sf::Sound>(ResourceManager::GetInstance().GetSoundBuffer(id));
    sound->setVolume(sfxVolume_);
    sound->play();
    activeSFX_.push_back(std::move(sound));
}

void SoundManager::setSFXVolume(float volume)
{
    sfxVolume_ = std::max(0.0f, std::min(100.0f, volume));
}

void SoundManager::update()
{
    activeSFX_.erase(
        std::remove_if(activeSFX_.begin(), activeSFX_.end(),
                       [](const std::unique_ptr<sf::Sound> &sound)
                       {
                           return sound->getStatus() == sf::Sound::Status::Stopped;
                       }),
        activeSFX_.end());
}

void SoundManager::updateBGM()
{
    if (!randomMode_ || !currentBGM_)
        return;
    if (currentBGM_->getStatus() == sf::Music::Status::Stopped)
    {
        playRandomBGM();
    }
}