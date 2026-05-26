#include "Arrow.h"
#include "Monster/Monster.h"

Arrow::Arrow()
{
    setSize({16, 16});
    setTextureId("arrow");
    setLifetime(1.5f);
    damage_ = 15;
    shape_.setOrigin({8, 8});
    shape_.setScale({3.0f, 3.0f});
}

void Arrow::onHit(Monster &monster)
{
    monster.takeDamage(damage_);
}
void Arrow::setVelocity(const sf::Vector2f &vel)
{
    Projectile::setVelocity(vel);

    // 计算速度方向角度
    float angleRad = std::atan2(vel.y, vel.x);
    float angleDeg = angleRad * 180 / 3.14159f;

    // 贴图默认朝右上角（45度），减去45度让箭头指向速度方向
    shape_.setRotation(sf::degrees(angleDeg + 45.0f));
}