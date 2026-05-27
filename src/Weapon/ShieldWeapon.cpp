#include "ShieldWeapon.h"
#include "../Monster/Monster.h"
#include "../Resourcemanager/ResourceManager.h"
#include <cmath>

ShieldWeapon::ShieldWeapon()
{
    name_ = "Shield";
    textureId_ = "shield";
    baseDamage_ = 10;
    damage_ = 10;
    attackCooldown_ = 0.0f;
    range_ = 100.0f;
    level_ = 1;

    radius_ = 80.0f;
    rotationSpeed_ = 3.0f;
    maxShields_ = 1;
    hitCooldown_ = 0.5f;

    // 加载盾牌纹理
    if (ResourceManager::GetInstance().HasTexture("shield"))
    {
        shieldTex_ = ResourceManager::GetInstance().GetTexture("shield");
        texSize_.x = shieldTex_.getSize().x; // 14
        texSize_.y = shieldTex_.getSize().y; // 23
    }

    createShield();
}

void ShieldWeapon::createShield()
{
    Shield newShield;

    // 设置纹理和大小
    newShield.shape.setTexture(&shieldTex_);
    float scale = 2.0f;
    newShield.shape.setSize({texSize_.x * scale, texSize_.y * scale});
    newShield.shape.setOrigin({texSize_.x * scale / 2.0f, texSize_.y * scale / 2.0f});

    // 均匀分布角度
    if (shields_.empty())
    {
        newShield.angle = 0;
    }
    else
    {
        float angleStep = 2 * 3.14159f / shields_.size();
        newShield.angle = shields_[0].angle + angleStep;
    }

    newShield.cooldown = 0.0f;
    newShield.canDamage = true;

    shields_.push_back(newShield);
}

void ShieldWeapon::update(float deltaTime)
{
    (void)deltaTime;
}

void ShieldWeapon::attack(const sf::Vector2f &playerPos,
                          const sf::Vector2f &attackDir,
                          std::vector<std::unique_ptr<Monster>> &monsters)
{
    (void)attackDir;
}

void ShieldWeapon::updateShields(float deltaTime, const sf::Vector2f &playerPos,
                                 std::vector<std::unique_ptr<Monster>> &monsters)
{
    for (auto &shield : shields_)
    {
        // 更新角度
        shield.angle += rotationSpeed_ * deltaTime;
        if (shield.angle > 2 * 3.14159f)
        {
            shield.angle -= 2 * 3.14159f;
        }

        // 更新冷却
        if (shield.cooldown > 0)
        {
            shield.cooldown -= deltaTime;
            if (shield.cooldown <= 0)
            {
                shield.canDamage = true;
            }
        }

        // 计算盾牌位置
        float x = playerPos.x + std::cos(shield.angle) * radius_;
        float y = playerPos.y + std::sin(shield.angle) * radius_;
        shield.shape.setPosition({x, y});

        // 设置旋转（盾牌朝向中心）
        float angleDeg = shield.angle * 180 / 3.14159f + 90.0f;
        shield.shape.setRotation(sf::degrees(angleDeg));

        // 碰撞检测
        if (shield.canDamage)
        {
            sf::FloatRect shieldBounds = shield.shape.getGlobalBounds();
            for (auto &monster : monsters)
            {
                if (!monster->isAlive())
                    continue;

                if (shieldBounds.findIntersection(monster->getBounds()).has_value())
                {
                    monster->takeDamage(damage_);
                    shield.canDamage = false;
                    shield.cooldown = hitCooldown_;
                    break;
                }
            }
        }
    }
}

void ShieldWeapon::levelUp()
{
    Weapon::levelUp();

    if (level_ == 2 && maxShields_ < 2)
    {
        maxShields_ = 2;
        createShield();
        float angleStep = 2 * 3.14159f / shields_.size();
        for (size_t i = 0; i < shields_.size(); i++)
        {
            shields_[i].angle = angleStep * i;
        }
    }
    else if (level_ == 4 && maxShields_ < 3)
    {
        maxShields_ = 3;
        createShield();
        float angleStep = 2 * 3.14159f / shields_.size();
        for (size_t i = 0; i < shields_.size(); i++)
        {
            shields_[i].angle = angleStep * i;
        }
    }
    else if (level_ == 6 && maxShields_ < 4)
    {
        maxShields_ = 4;
        createShield();
        float angleStep = 2 * 3.14159f / shields_.size();
        for (size_t i = 0; i < shields_.size(); i++)
        {
            shields_[i].angle = angleStep * i;
        }
    }

    // 升级提升属性
    damage_ += 2;
    radius_ += 5.0f;
    rotationSpeed_ += 0.5f;

    // 升级改变颜色（叠加颜色）
    int r = 100 + level_ * 10;
    int g = 150 + level_ * 5;
    sf::Color color(std::min(r, 255), std::min(g, 255), 200, 200);
    for (auto &shield : shields_)
    {
        shield.shape.setFillColor(color);
    }
}

void ShieldWeapon::render(sf::RenderWindow &window, const sf::Vector2f &playerPos) const
{
    (void)playerPos;
    for (const auto &shield : shields_)
    {
        window.draw(shield.shape);
    }
}