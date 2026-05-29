#include "ResourceManager.h"
#include "../Net/json.hpp"
#include <fstream>
#include <iostream>

ResourceManager &ResourceManager::GetInstance()
{
    static ResourceManager instance;
    return instance;
}

bool ResourceManager::LoadTexture(const std::string &id, const std::string &path)
{
    sf::Texture texture;
    if (!texture.loadFromFile(path))
    {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return false;
    }
    textures_[id] = std::move(texture);
    std::cout << "Loaded texture: " << id << " -> " << path << std::endl;
    return true;
}

bool ResourceManager::LoadFont(const std::string &id, const std::string &path)
{
    sf::Font font;
    if (!font.openFromFile(path))
    {
        std::cerr << "Failed to load font: " << path << std::endl;
        return false;
    }
    fonts_[id] = std::move(font);
    std::cout << "Loaded font: " << id << " -> " << path << std::endl;
    return true;
}

bool ResourceManager::LoadSoundBuffer(const std::string &id, const std::string &path)
{
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(path))
    {
        std::cerr << "Failed to load sound: " << path << std::endl;
        return false;
    }
    soundBuffers_[id] = std::move(buffer);
    std::cout << "Loaded sound: " << id << " -> " << path << std::endl;
    return true;
}

bool ResourceManager::LoadFromFile(const std::string &configPath)
{
    return LoadFromJsonFile(configPath);
}

bool ResourceManager::LoadFromJsonFile(const std::string &configPath)
{
    std::ifstream file(configPath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open JSON config: " << configPath << std::endl;
        return false;
    }

    nlohmann::json j;
    try
    {
        file >> j;
    }
    catch (const nlohmann::json::parse_error &e)
    {
        std::cerr << "JSON parse error in " << configPath << ": " << e.what() << std::endl;
        return false;
    }

    int loadedCount = 0;
    auto loadCategory = [&](const char *key, const std::string &type) {
        if (!j.contains(key))
            return;
        for (const auto &[id, path] : j[key].items())
        {
            std::string pathStr = path.get<std::string>();
            bool ok = false;
            if (type == "texture")
                ok = LoadTexture(id, pathStr);
            else if (type == "font")
                ok = LoadFont(id, pathStr);
            else if (type == "sound")
                ok = LoadSoundBuffer(id, pathStr);
            if (ok) loadedCount++;
        }
    };

    loadCategory("textures", "texture");
    loadCategory("fonts", "font");
    loadCategory("sounds", "sound");

    std::cout << "Loaded " << loadedCount << " resources from " << configPath << std::endl;
    return loadedCount > 0;
}

sf::Texture &ResourceManager::GetTexture(const std::string &id)
{
    return textures_.at(id);
}

sf::Font &ResourceManager::GetFont(const std::string &id)
{
    return fonts_.at(id);
}

sf::SoundBuffer &ResourceManager::GetSoundBuffer(const std::string &id)
{
    return soundBuffers_.at(id);
}

bool ResourceManager::HasTexture(const std::string &id) const
{
    return textures_.find(id) != textures_.end();
}

bool ResourceManager::HasFont(const std::string &id) const
{
    return fonts_.find(id) != fonts_.end();
}

bool ResourceManager::HasSoundBuffer(const std::string &id) const
{
    return soundBuffers_.find(id) != soundBuffers_.end();
}

void ResourceManager::Clear()
{
    textures_.clear();
    fonts_.clear();
    soundBuffers_.clear();
}