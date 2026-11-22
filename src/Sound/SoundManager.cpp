#include "SoundManager.h"
#include "Resourcemanager/ResourceManager.h"
#include "Config/SettingsStore.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace
{
// 武器音效（受"武器音量"控制）
const std::unordered_set<std::string> &weaponSfxIds()
{
    static const std::unordered_set<std::string> ids = {
        "sword_swing", "bow_shoot", "shield_block", "tnt_throw",
        "trident_throw", "axe_throw", "lava_throw", "seaheart_shoot",
        "dragon_breath", "mace_swing", "mace_hit",
        "lightning_strike_1", "lightning_strike_2", "lightning_strike_3",
        "madeinheaven"};
    return ids;
}

// 怪物音效（受"怪物音量"控制）
const std::unordered_set<std::string> &monsterSfxIds()
{
    static const std::unordered_set<std::string> ids = {
        "zombie_hurt", "zombie_death", "skeleton_hurt", "skeleton_death",
        "skeleton_shoot", "spider_hurt", "spider_death", "spider_shoot",
        "drowned_hurt", "drowned_death", "drowned_throw", "breeze_hurt",
        "breeze_death", "breeze_vortex", "creeper_hurt", "creeper_death",
        "creeper_fuse", "enderman_hurt", "enderman_death", "enderman_teleport",
        "witch_hurt", "witch_death", "witch_heal", "blaze_hurt",
        "blaze_death", "blaze_shoot", "ghast_hurt", "ghast_death",
        "ghast_shoot", "wither_hurt", "wither_death", "wither_shoot",
        "enderdragon_hurt", "enderdragon_death", "enderdragon_shoot",
        "enderdragon_laser", "wither_skeleton_hurt", "wither_skeleton_death",
        "slime_hurt", "slime_death", "slime_big_hurt", "slime_big_death",
        "boss_spawn"};
    return ids;
}
} // namespace

SoundManager &SoundManager::getInstance()
{
    static SoundManager instance;
    return instance;
}

SoundManager::SoundManager()
    : rng_(std::random_device{}()), bgmVolume_(20.0f), randomMode_(false), sfxVolume_(50.0f)
{
    // 读取玩家设置；首次运行没有文件、或文件损坏时用默认值并立刻写出一份
    const bool existed = settings_store::fileExists(kSettingsPath);
    bool parseFailed = false;
    const settings_store::GameSettings settings = settings_store::load(kSettingsPath, &parseFailed);

    masterVolume_ = static_cast<float>(settings_store::clampVolume(settings.audio.master));
    weaponVolume_ = static_cast<float>(settings_store::clampVolume(settings.audio.weapon));
    monsterVolume_ = static_cast<float>(settings_store::clampVolume(settings.audio.monster));
    bgmVolume_ = static_cast<float>(settings_store::clampVolume(settings.audio.bgm));
    sfxVolume_ = static_cast<float>(settings_store::clampVolume(settings.audio.sfx));

    if (!existed)
    {
        saveSettings();
        std::cout << "Settings file created: " << kSettingsPath << std::endl;
    }
    else if (parseFailed)
    {
        saveSettings();
        std::cout << "Settings file was invalid, rewritten with defaults: " << kSettingsPath << std::endl;
    }
    else
    {
        std::cout << "Settings loaded: " << kSettingsPath << " (master " << masterVolume_
                  << ", weapon " << weaponVolume_ << ", monster " << monsterVolume_
                  << ", bgm " << bgmVolume_ << ", sfx " << sfxVolume_ << ")" << std::endl;
    }

    std::cout << "SoundManager initialized" << std::endl;
}

void SoundManager::saveSettings() const
{
    // 先读一遍再写，免得把别的分组（比如 language）覆盖掉
    settings_store::GameSettings settings = settings_store::load(kSettingsPath);
    settings.audio.master = static_cast<int>(std::lround(masterVolume_));
    settings.audio.weapon = static_cast<int>(std::lround(weaponVolume_));
    settings.audio.monster = static_cast<int>(std::lround(monsterVolume_));
    settings.audio.bgm = static_cast<int>(std::lround(bgmVolume_));
    settings.audio.sfx = static_cast<int>(std::lround(sfxVolume_));
    settings_store::save(kSettingsPath, settings);
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
        currentBGM_->setVolume(bgmVolume_ * masterVolume_ / 100.0f);
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

void SoundManager::applyMusicVolume()
{
    if (currentBGM_)
    {
        currentBGM_->setVolume(bgmVolume_ * masterVolume_ / 100.0f);
    }
}

void SoundManager::setBGMVolume(float volume)
{
    bgmVolume_ = std::max(0.0f, std::min(100.0f, volume));
    applyMusicVolume();
    saveSettings();
}

void SoundManager::setMasterVolume(float volume)
{
    masterVolume_ = std::max(0.0f, std::min(100.0f, volume));
    applyMusicVolume();
    saveSettings();
}

void SoundManager::setWeaponVolume(float volume)
{
    weaponVolume_ = std::max(0.0f, std::min(100.0f, volume));
    saveSettings();
}

void SoundManager::setMonsterVolume(float volume)
{
    monsterVolume_ = std::max(0.0f, std::min(100.0f, volume));
    saveSettings();
}

// ========== 音效控制 ==========

void SoundManager::playSFX(const std::string &id, float volumeScale)
{
    if (!ResourceManager::GetInstance().HasSoundBuffer(id))
    {
        std::cout << "Warning: Sound buffer not found: " << id << std::endl;
        return;
    }

    // SFML 3 中 sf::Sound 需要传入 SoundBuffer 构造
    auto sound = std::make_unique<sf::Sound>(ResourceManager::GetInstance().GetSoundBuffer(id));

    // 分项音量：武器 / 怪物各自有倍率，其余（UI、拾取、升级等）走满倍率
    float categoryScale = 1.0f;
    if (weaponSfxIds().count(id))
    {
        categoryScale = weaponVolume_ / 100.0f;
    }
    else if (monsterSfxIds().count(id))
    {
        categoryScale = monsterVolume_ / 100.0f;
    }

    sound->setVolume(sfxVolume_ * (masterVolume_ / 100.0f) * categoryScale *
                     std::max(0.0f, std::min(1.0f, volumeScale)));
    sound->play();
    activeSFX_.push_back(std::move(sound));
}

void SoundManager::playSFXRandom(const std::vector<std::string> &ids, float volumeScale)
{
    if (ids.empty())
        return;

    // 只从确实存在的候选里挑，避免随机到缺失音效时静默失败
    std::vector<const std::string *> available;
    available.reserve(ids.size());
    for (const auto &id : ids)
    {
        if (ResourceManager::GetInstance().HasSoundBuffer(id))
            available.push_back(&id);
    }
    if (available.empty())
    {
        std::cout << "Warning: none of the sound variants exist" << std::endl;
        return;
    }

    std::uniform_int_distribution<std::size_t> pick(0, available.size() - 1);
    playSFX(*available[pick(rng_)], volumeScale);
}

void SoundManager::setSFXVolume(float volume)
{
    sfxVolume_ = std::max(0.0f, std::min(100.0f, volume));
    saveSettings();
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