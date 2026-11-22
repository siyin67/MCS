#include "Localization.h"
#include "Config/SettingsStore.h"
#include "Net/json.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

Localization &Localization::getInstance()
{
    static Localization instance;
    return instance;
}

void Localization::initialize(const std::string &langDir, const std::string &settingsPath)
{
    langDir_ = langDir;
    settingsPath_ = settingsPath;
    tables_.clear();
    languages_.clear();

    namespace fs = std::filesystem;
    std::error_code ec;
    if (fs::is_directory(langDir_, ec))
    {
        for (const fs::directory_entry &entry : fs::directory_iterator(langDir_, ec))
        {
            if (!entry.is_regular_file(ec) || entry.path().extension() != ".json")
            {
                continue;
            }
            loadFile(entry.path().string(), entry.path().stem().string());
        }
    }

    if (languages_.empty())
    {
        std::cerr << "[Lang] No language file found in " << langDir_ << std::endl;
        languages_.push_back({"en_us", "English (US)", "English (US)"});
    }

    std::sort(languages_.begin(), languages_.end(),
              [](const LanguageInfo &a, const LanguageInfo &b) { return a.code < b.code; });

    // 取设置里记住的语言；文件被删了就用第一个
    const settings_store::GameSettings settings = settings_store::load(settingsPath_);
    current_ = settings.language;
    if (tables_.find(current_) != tables_.end())
    {
        const auto it = std::find_if(languages_.begin(), languages_.end(),
                                     [this](const LanguageInfo &l) { return l.code == current_; });
        if (it != languages_.end())
        {
            std::cout << "Language loaded: " << current_ << " (" << it->display << ")" << std::endl;
        }
        return;
    }

    current_ = languages_.front().code;
    std::cout << "Language loaded: " << current_ << " (fallback)" << std::endl;
    saveLanguage();
}

void Localization::loadFile(const std::string &path, const std::string &code)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "[Lang] Cannot open " << path << std::endl;
        return;
    }

    try
    {
        nlohmann::json j;
        file >> j;

        LanguageInfo info;
        info.code = code;
        info.display = (j.contains("display") && j["display"].is_string())
                           ? j["display"].get<std::string>()
                           : code;
        info.english = (j.contains("english") && j["english"].is_string())
                           ? j["english"].get<std::string>()
                           : info.display;

        std::map<std::string, std::string> table;
        if (j.contains("strings") && j["strings"].is_object())
        {
            for (auto it = j["strings"].begin(); it != j["strings"].end(); ++it)
            {
                if (it.value().is_string())
                {
                    table[it.key()] = it.value().get<std::string>();
                }
            }
        }

        tables_[code] = std::move(table);
        languages_.push_back(std::move(info));
    }
    catch (const nlohmann::json::parse_error &e)
    {
        std::cerr << "[Lang] Parse error in " << path << ": " << e.what() << std::endl;
    }
}

bool Localization::setLanguage(const std::string &code)
{
    if (tables_.find(code) == tables_.end())
    {
        std::cerr << "[Lang] Unknown language: " << code << std::endl;
        return false;
    }
    if (current_ == code)
    {
        return true;
    }

    current_ = code;
    ++version_;
    saveLanguage();
    std::cout << "Language switched: " << current_ << std::endl;
    return true;
}

void Localization::saveLanguage() const
{
    // 只改 language 字段，其它分组（音量等）原样保留
    settings_store::GameSettings settings = settings_store::load(settingsPath_);
    settings.language = current_;
    settings_store::save(settingsPath_, settings);
}

std::string Localization::tr(const std::string &key) const
{
    const auto cur = tables_.find(current_);
    if (cur != tables_.end())
    {
        const auto it = cur->second.find(key);
        if (it != cur->second.end())
        {
            return it->second;
        }
    }

    const auto fallback = tables_.find("en_us");
    if (fallback != tables_.end())
    {
        const auto it = fallback->second.find(key);
        if (it != fallback->second.end())
        {
            return it->second;
        }
    }

    return key;
}

bool Localization::has(const std::string &key) const
{
    const auto cur = tables_.find(current_);
    if (cur != tables_.end() && cur->second.find(key) != cur->second.end())
    {
        return true;
    }
    const auto fallback = tables_.find("en_us");
    return fallback != tables_.end() && fallback->second.find(key) != fallback->second.end();
}

std::string Localization::trOr(const std::string &key, const std::string &fallbackText) const
{
    return has(key) ? tr(key) : fallbackText;
}

static std::string fillPlaceholders(const std::string &templ, const std::vector<std::string> &args)
{
    std::string out = templ;
    for (std::size_t i = 0; i < args.size(); ++i)
    {
        const std::string token = "{" + std::to_string(i) + "}";
        std::size_t pos = 0;
        while ((pos = out.find(token, pos)) != std::string::npos)
        {
            out.replace(pos, token.size(), args[i]);
            pos += args[i].size();
        }
    }
    return out;
}

std::string Localization::trf(const std::string &key, const std::vector<std::string> &args) const
{
    return fillPlaceholders(tr(key), args);
}

std::string Localization::trfOr(const std::string &key, const std::string &fallbackText,
                                const std::vector<std::string> &args) const
{
    return fillPlaceholders(trOr(key, fallbackText), args);
}
