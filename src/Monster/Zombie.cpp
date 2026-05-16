#include "Zombie.h"
#include "Resourcemanager/ResourceManager.h"

Zombie::Zombie()
{
    speed_ = 80.0f;
    health_ = 30;
    maxHealth_ = 30;
    damage_ = 10;
    expReward_ = 5;
    if (ResourceManager::GetInstance().HasTexture("zombie"))
    {
        sf::Texture &bgTex = ResourceManager::GetInstance().GetTexture("zombie");
        shape_.setTexture(&bgTex);
    }

    shape_.setSize({64, 64});
}