#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>

class ResourceManager
{
private:
    std::unordered_map<std::string, sf::Texture> textures_;
    std::unordered_map<std::string, sf::Font> fonts_;
    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers_;

    ResourceManager() = default;
    ResourceManager(const ResourceManager &) = delete;
    ResourceManager &operator=(const ResourceManager &) = delete;

    bool LoadFont(const std::string &id, const std::string &path);
    bool LoadSoundBuffer(const std::string &id, const std::string &path);

public:
    static ResourceManager &GetInstance();

    bool LoadFromFile(const std::string &configPath);
    bool LoadFromJsonFile(const std::string &configPath);
    bool LoadTexture(const std::string &id, const std::string &path);

    sf::Texture &GetTexture(const std::string &id);
    sf::Font &GetFont(const std::string &id);
    sf::SoundBuffer &GetSoundBuffer(const std::string &id);

    bool HasTexture(const std::string &id) const;
    bool HasFont(const std::string &id) const;
    bool HasSoundBuffer(const std::string &id) const;

    void Clear();
};