#include "Spider.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"
#include "../Projectile/MonsterProjectile.h"
#include <cmath>

Spider::Spider()
{
    auto cfg = loadJsonConfig("assets/config/monster/spider.json");
    speed_ = cfg.value("speed", 150.0f);
    health_ = cfg.value("health", 20);
    maxHealth_ = cfg.value("maxHealth", 20);
    damage_ = cfg.value("damage", 1);
    expReward_ = cfg.value("expReward", 3);
    std::string texId = cfg.value("textureId", "spider");
    if (ResourceManager::GetInstance().HasTexture(texId))
    {
        sf::Texture &tex = ResourceManager::GetInstance().GetTexture(texId);
        shape_.setTexture(&tex);
    }
    shape_.setSize({cfg.value("size", std::vector<float>{64, 64})[0], cfg.value("size", std::vector<float>{64, 64})[1]});

    float skillCD = cfg.value("skillCooldown", 10.0f);
    float webSpeed = cfg.value("webSpeed", 250.0f);
    float webLifetime = cfg.value("webLifetime", 2.0f);
    int webSize = cfg.value("webSize", 24);
    float slowValue = cfg.value("slowValue", 0.7f);
    float slowDuration = cfg.value("slowDuration", 3.0f);
    setSkill([webSpeed, webLifetime, webSize, slowValue, slowDuration](Monster &self, const sf::Vector2f &playerPos)
    {
        sf::Vector2f dir = playerPos - self.getPosition();
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0.01f) { dir.x /= len; dir.y /= len; }
        auto web = std::make_unique<MonsterProjectile>();
        web->setPosition(self.getPosition());
        web->setDirection(dir);
        web->setSpeed(webSpeed);
        web->setDamage(self.getDamage());
        web->setLifetime(webLifetime);
        web->setTextureId("web");
        web->setSize({(float)webSize, (float)webSize});
        web->setHitBuff(BuffType::SpeedDown, slowValue, slowDuration);
        Monster::pendingProjectiles.push_back(std::move(web));
        SoundManager::getInstance().playSFX("spider_shoot");
    }, skillCD);
}
void Spider::playHitSound()
{
    SoundManager::getInstance().playSFX("spider_hurt");
}
void Spider::playDeathSound()
{
    SoundManager::getInstance().playSFX("spider_death");
}
