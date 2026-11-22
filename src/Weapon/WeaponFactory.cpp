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
#include "MaceWeapon.h"
#include "LightningRodWeapon.h"

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
    registerWeapon("mace", []()
                   { return std::make_unique<MaceWeapon>(); });
    registerWeapon("lightingrod", []()
                   { return std::make_unique<LightningRodWeapon>(); });
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
        auto weapon = it->second();
        if (weapon)
        {
            weapon->setId(id); // 记住工厂 key，界面用 weapon.<id>.name 取本地化名字
        }
        return weapon;
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