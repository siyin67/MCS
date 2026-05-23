#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

#include "Weapon.h"

class WeaponFactory
{
private:
    std::unordered_map<std::string, std::function<std::unique_ptr<Weapon>()>> creators_;

    WeaponFactory();
    void registerDefaults();
    void registerWeapon(const std::string &id, std::function<std::unique_ptr<Weapon>()> creator);

public:
    static WeaponFactory &getInstance();

    std::unique_ptr<Weapon> create(const std::string &id);

    bool hasWeapon(const std::string &id) const;

    std::vector<std::string> getAllWeaponIds() const;
};