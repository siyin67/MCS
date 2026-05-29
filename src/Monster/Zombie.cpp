#include "Zombie.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Config/ConfigLoader.h"

Zombie::Zombie()
{
    auto cfg = loadJsonConfig("assets/config/monster/zombie.json");
    speed_ = cfg.value("speed", 80.0f);
    health_ = cfg.value("health", 30);
    maxHealth_ = cfg.value("maxHealth", 30);
    damage_ = cfg.value("damage", 1);
    expReward_ = cfg.value("expReward", 5);
    std::string texId = cfg.value("textureId", "zombie");
    if (ResourceManager::GetInstance().HasTexture(texId))
    {
        sf::Texture &bgTex = ResourceManager::GetInstance().GetTexture(texId);
        shape_.setTexture(&bgTex);
    }
    shape_.setSize({cfg.value("size", std::vector<float>{64, 64})[0], cfg.value("size", std::vector<float>{64, 64})[1]});
}
void Zombie::playHitSound()
{
    SoundManager::getInstance().playSFX("zombie_hurt");
}
void Zombie::playDeathSound()
{
    SoundManager::getInstance().playSFX("zombie_death");
}
