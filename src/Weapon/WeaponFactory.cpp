#include "WeaponFactory.h"
#include "SwordWeapon.h"
#include "Weapon.h"

WeaponFactory::WeaponFactory()
{
    registerDefaults();
}

void WeaponFactory::registerDefaults()
{
    registerWeapon("sword", []()
                   { return std::make_unique<SwordWeapon>(); });
}

void WeaponFactory::registerWeapon(const std::string &id,
                                   std::function<std::unique_ptr<Weapon>()> creator)
{
    creators_[id] = creator;
}

WeaponFactory &WeaponFactory::getInstance()
{
    static WeaponFactory instance;
    return instance;
}

std::unique_ptr<Weapon> WeaponFactory::create(const std::string &id)
{
    auto it = creators_.find(id);
    if (it != creators_.end())
    {
        return it->second();
    }
    return nullptr;
}

bool WeaponFactory::hasWeapon(const std::string &id) const
{
    return creators_.find(id) != creators_.end();
}

std::vector<std::string> WeaponFactory::getAllWeaponIds() const
{
    std::vector<std::string> ids;
    for (const auto &pair : creators_)
    {
        ids.push_back(pair.first);
    }
    return ids;
}