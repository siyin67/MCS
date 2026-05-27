#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <utility>
#include <cmath>
#include <algorithm>

// ============================================================================
// 碰撞类型枚举
// ============================================================================
enum class CollisionType
{
    PLAYER,
    MONSTER,
    BULLET,
    DROPITEM
};

// ============================================================================
// ICollidable 接口
// 所有可碰撞对象（Player、Monster、Bullet）需继承此接口
// ============================================================================
class ICollidable
{
public:
    virtual ~ICollidable() = default;

    // 返回 AABB 包围盒，用于快速筛选
    virtual sf::FloatRect getGlobalBounds() const = 0;

    // 返回多边形顶点（世界坐标），用于 SAT 精确检测
    // 对于矩形对象，返回4个角点；对于复杂形状，返回实际顶点
    virtual std::vector<sf::Vector2f> getVertices() const = 0;

    // 返回碰撞类型
    virtual CollisionType getType() const = 0;
};

// ============================================================================
// 碰撞检测函数声明
// ============================================================================

// AABB 快速排除：检测两个包围盒是否相交
bool aabbCollide(const sf::FloatRect &a, const sf::FloatRect &b);

// 分离轴定理（SAT）精确检测：仅处理凸多边形
bool satCollide(const std::vector<sf::Vector2f> &vertsA, const std::vector<sf::Vector2f> &vertsB);

// 根据碰撞类型决定检测策略：
//   - Player-Monster：仅 AABB
//   - 涉及 Bullet：AABB + SAT
bool checkCollision(ICollidable *objA, ICollidable *objB);

// 批量检测：返回所有发生碰撞的对象对（不重复）
std::vector<std::pair<ICollidable *, ICollidable *>> detectCollisions(const std::vector<ICollidable *> &objects);

// ============================================================================
// 工具函数：从 FloatRect 生成4个角点（用于矩形对象的 getVertices）
// ============================================================================
inline std::vector<sf::Vector2f> verticesFromBounds(const sf::FloatRect &bounds)
{
    float left = bounds.position.x;
    float top = bounds.position.y;
    float right = left + bounds.size.x;
    float bottom = top + bounds.size.y;

    return {
        {left, top},    // 左上
        {right, top},   // 右上
        {right, bottom},// 右下
        {left, bottom}  // 左下
    };
}
