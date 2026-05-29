#pragma once
#include "../Net/json.hpp"
#include <fstream>
#include <iostream>
#include <string>

inline nlohmann::json loadJsonConfig(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "[ConfigLoader] Failed to open: " << path << std::endl;
        return {};
    }
    try
    {
        nlohmann::json j;
        file >> j;
        return j;
    }
    catch (const nlohmann::json::parse_error &e)
    {
        std::cerr << "[ConfigLoader] Parse error in " << path << ": " << e.what() << std::endl;
        return {};
    }
}
