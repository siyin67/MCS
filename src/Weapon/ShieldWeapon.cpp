#include "ShieldWeapon.h"
#include "../Monster/Monster.h"
#include "../Resourcemanager/ResourceManager.h"
#include "../Config/ConfigLoader.h"
#include "../Sound/SoundManager.h"
#include <cmath>

ShieldWeapon::ShieldWeapon()
{
    auto cfg = loadJsonConfig("assets/config/weapon/shield.json");
    name_ = cfg.value("name", "Shield");
    textureId_ = cfg.value("textureId", "shield");
    baseDamage_ = cfg.value("baseDamage", 10);
    damage_ = cfg.value("damage", 10);
    attackCooldown_ = cfg.value("attackCooldown", 0.0f);
    range_ = cfg.value("range", 100.0f);
    level_ = 1;

    radius_ = cfg.value("radius", 80.0f);
    rotationSpeed_ = cfg.value("rotationSpeed", 3.0f);
    maxShields_ = cfg.value("maxShields", 1);
    hitCooldown_ = cfg.value("hitCooldown", 0.5f);
    levelUpCfg_ = cfg.value("levelUp", nlohmann::json{});

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
                    SoundManager::getInstance().playSFX("shield_block");
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

    // 读取盾牌数量升级点
    if (!levelUpCfg_.empty() && levelUpCfg_.contains("maxShieldsAt"))
    {
        for (const auto &v : levelUpCfg_["maxShieldsAt"])
        {
            int lv = v.get<int>();
            if (level_ == lv && maxShields_ < lv)
            {
                maxShields_ = lv;
                createShield();
                float angleStep = 2 * 3.14159f / shields_.size();
                for (size_t i = 0; i < shields_.size(); i++)
                    shields_[i].angle = angleStep * i;
                break;
            }
        }
    }

    // 额外属性
    int dmg = levelUpCfg_.value("damage", 2);
    damage_ += dmg;
    radius_ += levelUpCfg_.value("radius", 5.0f);
    rotationSpeed_ += 0.5f;

    int r = 100 + level_ * 10;
    int g = 150 + level_ * 5;
    sf::Color color(std::min(r, 255), std::min(g, 255), 200, 200);
    for (auto &shield : shields_)
        shield.shape.setFillColor(color);
}

void ShieldWeapon::render(sf::RenderWindow &window, const sf::Vector2f &playerPos) const
{
    (void)playerPos;
    for (const auto &shield : shields_)
    {
        window.draw(shield.shape);
    }
}