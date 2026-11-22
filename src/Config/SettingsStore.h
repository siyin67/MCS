#pragma once
// 玩家设置持久化：assets/config/settings.json
// 结构按“分组”组织，以后加视频/按键等设置直接在同一份 json 里开新分组，
// 缺字段时自动落回默认值（老版本存档也能读）。
#include "../Net/json.hpp"
#include <fstream>
#include <iostream>
#include <string>

namespace settings_store
{
inline constexpr const char *kDefaultPath = "assets/config/settings.json";

struct AudioSettings
{
    int master = 100;
    int weapon = 100;
    int monster = 100;
    int bgm = 20;
    int sfx = 50;
};

struct GameSettings
{
    AudioSettings audio;
    std::string language = "zh_cn"; // assets/lang/<code>.json
};

inline int clampVolume(int v) { return v < 0 ? 0 : (v > 100 ? 100 : v); }

inline bool fileExists(const std::string &path)
{
    std::ifstream probe(path);
    return probe.good();
}

inline GameSettings load(const std::string &path, bool *parseFailed = nullptr)
{
    GameSettings s; // 默认值
    if (parseFailed)
    {
        *parseFailed = false;
    }
    std::ifstream file(path);
    if (!file.is_open())
    {
        return s;
    }

    try
    {
        nlohmann::json j;
        file >> j;
        if (j.contains("audio") && j["audio"].is_object())
        {
            const nlohmann::json &a = j["audio"];
            auto readInt = [&a](const char *key, int fallback)
            {
                if (a.contains(key) && (a[key].is_number_integer() || a[key].is_number_unsigned()))
                {
                    return clampVolume(a[key].get<int>());
                }
                return fallback;
            };
            s.audio.master = readInt("master", s.audio.master);
            s.audio.weapon = readInt("weapon", s.audio.weapon);
            s.audio.monster = readInt("monster", s.audio.monster);
            s.audio.bgm = readInt("bgm", s.audio.bgm);
            s.audio.sfx = readInt("sfx", s.audio.sfx);
        }
        if (j.contains("language") && j["language"].is_string())
        {
            s.language = j["language"].get<std::string>();
        }
    }
    catch (const nlohmann::json::parse_error &e)
    {
        std::cerr << "[Settings] Parse error in " << path << ": " << e.what()
                  << " (falling back to defaults)" << std::endl;
        if (parseFailed)
        {
            *parseFailed = true;
        }
    }

    return s;
}

inline bool save(const std::string &path, const GameSettings &s)
{
    nlohmann::json j;
    j["language"] = s.language;
    j["audio"] = {
        {"master", clampVolume(s.audio.master)},
        {"weapon", clampVolume(s.audio.weapon)},
        {"monster", clampVolume(s.audio.monster)},
        {"bgm", clampVolume(s.audio.bgm)},
        {"sfx", clampVolume(s.audio.sfx)},
    };

    std::ofstream file(path);
    if (!file.is_open())
    {
        std::cerr << "[Settings] Failed to write: " << path << std::endl;
        return false;
    }
    file << j.dump(2) << std::endl;
    return true;
}
} // namespace settings_store
