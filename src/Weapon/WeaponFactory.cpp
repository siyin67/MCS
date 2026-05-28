#include "WeaponFactory.h"
#include "SwordWeapon.h"
#include "BowWeapon.h"
#include "ShieldWeapon.h"
#include "TNTWeapon.h"
#include "TridentWeapon.h"
#include "ClockWeapon.h"
#include "AxeWeapon.h"
#include "DragonBreathWeapon.h"
#include "SeaHeartWeapon.h"
#include "LavaWeapon.h"

#include "Weapon.h"

WeaponFactory::WeaponFactory()
{
    registerDefaults();
}

void WeaponFactory::registerDefaults()
{
    registerWeapon("sword", []()
                   { return std::make_unique<SwordWeapon>(); });
    registerWeapon("bow", []()
                   { return std::make_unique<BowWeapon>(); });
    registerWeapon("shield", []()
                   { return std::make_unique<ShieldWeapon>(); });
    registerWeapon("tnt", []()
                   { return std::make_unique<TNTWeapon>(); });
    registerWeapon("trident", []()
                   { return std::make_unique<TridentWeapon>(); });
    registerWeapon("clock", []()
                   { return std::make_unique<ClockWeapon>(); });
    registerWeapon("axe", []()
                   { return std::make_unique<AxeWeapon>(); });
    registerWeapon("dragon", []()
                   { return std::make_unique<DragonBreathWeapon>(); });
    registerWeapon("seaheart", []()
                   { return std::make_unique<SeaHeartWeapon>(); });
    registerWeapon("lava", []()
                   { return std::make_unique<LavaWeapon>(); });
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