
#include "CollisionResolver.h"
#include "Player/player.h"
#include "Monster/Monster.h"
#include <cmath>

// ============================================================================
// 处理所有碰撞对的游戏逻辑
// ============================================================================
CollisionResolveResult resolveCollisions(
    const std::vector<std::pair<ICollidable *, ICollidable *>> &collisionPairs,
    Player &player)
{
    CollisionResolveResult result;

    for (const auto &[objA, objB] : collisionPairs)
    {
        CollisionType typeA = objA->getType();
        CollisionType typeB = objB->getType();

        // ---------------------------------------------------------------
        // Player-Monster 碰撞：分发到各自的碰撞处理函数
        // ---------------------------------------------------------------
        if ((typeA == CollisionType::PLAYER && typeB == CollisionType::MONSTER) ||
            (typeB == CollisionType::PLAYER && typeA == CollisionType::MONSTER))
        {
            auto *monster = static_cast<Monster *>(
                typeA == CollisionType::MONSTER ? objA : objB);

            // 击退方向：从怪物指向玩家（归一化）
            sf::Vector2f knockbackDir = player.getPosition() - monster->getPosition();
            float len = std::sqrt(knockbackDir.x * knockbackDir.x + knockbackDir.y * knockbackDir.y);
            if (len > 0.01f)
            {
                knockbackDir.x /= len;
                knockbackDir.y /= len;
            }
            else
            {
                knockbackDir = {0.0f, -1.0f}; // 默认向上击退
            }

            // 分发：各类自己处理碰撞逻辑
            player.onCollideWithMonster(*monster, knockbackDir);
            monster->onCollideWithPlayer(player, knockbackDir);

            result.playerDamaged = true;
            result.playerDamageTaken += monster->getDamage();
        }

        // ---------------------------------------------------------------
        // Bullet-Monster 碰撞：子弹对怪物造成伤害（预留扩展）
        // ---------------------------------------------------------------
        // else if (involvesBullet(typeA, typeB) &&
        //          (typeA == CollisionType::MONSTER || typeB == CollisionType::MONSTER))
        // {
        //     auto *monster = static_cast<Monster *>(
        //         typeA == CollisionType::MONSTER ? objA : objB);
        //     auto *bullet  = static_cast<Bullet *>(
        //         typeA == CollisionType::BULLET ? objA : objB);
        //     monster->takeDamage(bullet->getDamage());
        //     bullet->markForRemoval();
        // }
    }

    return result;
}

// ============================================================================
// 清理死亡怪物，返回被移除的数量
// ============================================================================
int removeDeadMonsters(std::vector<std::unique_ptr<Monster>> &monsters,
					   std::function<void(sf::Vector2f, int)> onDrop)
{
	int removed = 0;
	for (auto it = monsters.begin(); it != monsters.end();)
	{
		if (!(*it)->isAlive())
		{
			(*it)->playDeathSound();
			if (onDrop)
				onDrop((*it)->getPosition(), (*it)->getExpReward());
			it = monsters.erase(it);
			++removed;
		}
		else
		{
			++it;
		}
	}
	return removed;
}
