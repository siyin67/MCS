#include "Skeleton.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"
#include "../Projectile/Arrow.h"
#include <cmath>

Skeleton::Skeleton()
{
    auto cfg = loadJsonConfig("assets/config/monster/skeleton.json");
    speed_ = cfg.value("speed", 100.0f);
    health_ = cfg.value("health", 25);
    maxHealth_ = cfg.value("maxHealth", 25);
    damage_ = cfg.value("damage", 1);
    expReward_ = cfg.value("expReward", 4);
    std::string texId = cfg.value("textureId", "skeleton");
    if (ResourceManager::GetInstance().HasTexture(texId))
    {
        sf::Texture &tex = ResourceManager::GetInstance().GetTexture(texId);
        shape_.setTexture(&tex);
    }
    shape_.setSize({cfg.value("size", std::vector<float>{64, 64})[0], cfg.value("size", std::vector<float>{64, 64})[1]});

    float skillCD = cfg.value("skillCooldown", 10.0f);
    float arrowSpeed = cfg.value("projectileSpeed", 350.0f);
    setSkill([arrowSpeed](Monster &self, const sf::Vector2f &playerPos)
    {
        sf::Vector2f dir = playerPos - self.getPosition();
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0.01f) { dir.x /= len; dir.y /= len; }
        auto arrow = std::make_unique<Arrow>();
        arrow->setPosition(self.getPosition());
        arrow->setVelocity(dir * arrowSpeed);
        arrow->setDamage(self.getDamage());
        arrow->setLifetime(3.0f);
        Monster::pendingProjectiles.push_back(std::move(arrow));
        SoundManager::getInstance().playSFX("skeleton_shoot");
    }, skillCD);
}
void Skeleton::playHitSound()
{
    SoundManager::getInstance().playSFX("skeleton_hurt");
}
void Skeleton::playDeathSound()
{
    SoundManager::getInstance().playSFX("skeleton_death");
}
