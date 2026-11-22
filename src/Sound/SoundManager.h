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

    // 分项音量（0~100）：最终音量 = 分项音量/100 * 主音量/100
    float masterVolume_ = 100.0f;
    float weaponVolume_ = 100.0f;
    float monsterVolume_ = 100.0f;

    // 设置落盘路径
    static constexpr const char *kSettingsPath = "assets/config/settings.json";

    void applyMusicVolume();

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
    float getBGMVolume() const { return bgmVolume_; }

    // 音效控制
    // volumeScale: 0.0~1.0 的相对音量，只作用于这一条音效，不改动全局 SFX 音量
    void playSFX(const std::string &id, float volumeScale = 1.0f);
    // 从一组同义音效里随机挑一个播放（例如雷击的 thunder1/2/3），避免每次都一模一样
    void playSFXRandom(const std::vector<std::string> &ids, float volumeScale = 1.0f);
    void setSFXVolume(float volume);
    float getSFXVolume() const { return sfxVolume_; }

    // 分项音量（设置界面用）
    void setMasterVolume(float volume);
    float getMasterVolume() const { return masterVolume_; }
    void setWeaponVolume(float volume);
    float getWeaponVolume() const { return weaponVolume_; }
    void setMonsterVolume(float volume);
    float getMonsterVolume() const { return monsterVolume_; }

    // 把所有音量写回 assets/config/settings.json（任何 setter 改动后会自动调用）
    void saveSettings() const;

    // 更新
    void update();
    void updateBGM();
};