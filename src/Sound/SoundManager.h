#pragma once
#include <SFML/Audio.hpp>
#include <memory>
#include <vector>
#include <string>
#include <random>

class SoundManager
{
private:
    // BGM
    std::unique_ptr<sf::Music> currentBGM_;
    std::vector<std::string> bgmPlaylist_;
    std::mt19937 rng_;
    float bgmVolume_;
    bool randomMode_;

    // 音效
    std::vector<std::unique_ptr<sf::Sound>> activeSFX_;
    float sfxVolume_;

    SoundManager();
    ~SoundManager() = default;

public:
    static SoundManager &getInstance();

    // BGM 控制
    void setBGMPlaylist(const std::vector<std::string> &tracks);
    void playRandomBGM();
    void playBGM(const std::string &path, bool loop = true);
    void stopBGM();
    void pauseBGM();
    void resumeBGM();
    void setBGMVolume(float volume);

    // 音效控制
    void playSFX(const std::string &id);
    void setSFXVolume(float volume);

    // 更新
    void update();
    void updateBGM();
};