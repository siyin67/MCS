
#pragma once
#include "Collision.h"
#include <vector>
#include <memory>
#include <utility>

// 前向声明
class Player;
class Monster;

// ============================================================================
// 碰撞解决器：负责处理碰撞检测后的逻辑变更
// ============================================================================

// 碰撞解决结果
struct CollisionResolveResult
{
    bool playerDamaged = false;   // 玩家是否受伤
    int  playerDamageTaken = 0;   // 玩家受到的总伤害
    int  monstersKilled = 0;      // 被击杀的怪物数量
};

// 处理所有碰撞对的游戏逻辑
//   - Player-Monster：怪物对玩家造成伤害
//   - Bullet-Monster：子弹对怪物造成伤害（预留）
CollisionResolveResult resolveCollisions(
    const std::vector<std::pair<ICollidable *, ICollidable *>> &collisionPairs,
    Player &player);

// 清理死亡怪物，返回被移除的数量
int removeDeadMonsters(std::vector<std::unique_ptr<Monster>> &monsters);
