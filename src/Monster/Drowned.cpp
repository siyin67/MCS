#include "Drowned.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"
#include "../Projectile/TridentProjectile.h"
#include <cmath>

Drowned::Drowned()
{
    auto cfg = loadJsonConfig("assets/config/monster/drowned.json");
    speed_ = cfg.value("speed", 85.0f);
    health_ = cfg.value("health", 35);
    maxHealth_ = cfg.value("maxHealth", 35);
    damage_ = cfg.value("damage", 1);
    expReward_ = cfg.value("expReward", 6);
    std::string texId = cfg.value("textureId", "drowned");
    if (ResourceManager::GetInstance().HasTexture(texId))
    {
        sf::Texture &tex = ResourceManager::GetInstance().GetTexture(texId);
        shape_.setTexture(&tex);
    }
    shape_.setSize({64, 64});

    float skillCD = cfg.value("skillCooldown", 10.0f);
    float tridentSpeed = cfg.value("tridentSpeed", 600.0f);
    float returnSpeed = cfg.value("tridentReturnSpeed", 350.0f);
    setSkill([tridentSpeed, returnSpeed](Monster &self, const sf::Vector2f &playerPos)
    {
        sf::Vector2f dir = playerPos - self.getPosition();
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0.01f) { dir.x /= len; dir.y /= len; }
        auto trident = std::make_unique<TridentProjectile>(
            self.getPosition(), dir, self.getDamage(), tridentSpeed, returnSpeed);
        Monster::pendingProjectiles.push_back(std::move(trident));
        SoundManager::getInstance().playSFX("drowned_throw");
    }, skillCD);
}
void Drowned::playHitSound()
{
    SoundManager::getInstance().playSFX("drowned_hurt");
}
void Drowned::playDeathSound()
{
    SoundManager::getInstance().playSFX("drowned_death");
}
