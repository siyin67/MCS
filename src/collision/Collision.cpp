#include "Collision.h"

// ============================================================================
// AABB 快速排除
// 直接调用 SFML 的 FloatRect::intersects()
// ============================================================================
bool aabbCollide(const sf::FloatRect &a, const sf::FloatRect &b)
{
    // SFML3 中 intersects() 已重命名为 findIntersection()，返回 std::optional
    return a.findIntersection(b).has_value();
}

// ============================================================================
// SAT 辅助：将顶点投影到指定轴上，返回投影区间 [min, max]
// ============================================================================
static void projectOntoAxis(const std::vector<sf::Vector2f> &vertices,
                            const sf::Vector2f &axis,
                            float &min, float &max)
{
    min = max = vertices[0].x * axis.x + vertices[0].y * axis.y;
    for (size_t i = 1; i < vertices.size(); ++i)
    {
        float projection = vertices[i].x * axis.x + vertices[i].y * axis.y;
        if (projection < min) min = projection;
        if (projection > max) max = projection;
    }
}

// ============================================================================
// SAT 辅助：从边生成法线轴（垂直于边的方向）
// ============================================================================
static void addAxesFromVertices(const std::vector<sf::Vector2f> &vertices,
                                std::vector<sf::Vector2f> &axes)
{
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        size_t j = (i + 1) % vertices.size();
        sf::Vector2f edge = vertices[j] - vertices[i];

        // 法线（垂直于边）：(-edge.y, edge.x)
        sf::Vector2f normal(-edge.y, edge.x);

        // 归一化
        float len = std::sqrt(normal.x * normal.x + normal.y * normal.y);
        if (len > 0.0001f)
        {
            normal.x /= len;
            normal.y /= len;
        }

        axes.push_back(normal);
    }
}

// ============================================================================
// 分离轴定理（SAT）精确检测
// 仅处理凸多边形，对矩形和三角形等足够
// ============================================================================
bool satCollide(const std::vector<sf::Vector2f> &vertsA, const std::vector<sf::Vector2f> &vertsB)
{
    if (vertsA.size() < 2 || vertsB.size() < 2)
        return false;

    std::vector<sf::Vector2f> axes;

    // 收集所有分离轴：两个多边形所有边的法线
    addAxesFromVertices(vertsA, axes);
    addAxesFromVertices(vertsB, axes);

    // 在每个轴上检测投影是否分离
    for (const auto &axis : axes)
    {
        float minA, maxA, minB, maxB;
        projectOntoAxis(vertsA, axis, minA, maxA);
        projectOntoAxis(vertsB, axis, minB, maxB);

        // 如果任一轴上投影不重叠，则不存在碰撞
        if (maxA < minB || maxB < minA)
            return false;
    }

    // 所有轴上投影都重叠，确认碰撞
    return true;
}

// ============================================================================
// 判断碰撞对是否涉及 Bullet
// ============================================================================
static bool involvesBullet(CollisionType typeA, CollisionType typeB)
{
    return typeA == CollisionType::BULLET || typeB == CollisionType::BULLET;
}

// ============================================================================
// 根据碰撞类型决定检测策略
//   - Player-Monster：仅使用 AABB 检测（矩形对矩形，AABB 已足够精确）
//   - 涉及 Bullet（Bullet-Player / Bullet-Monster）：AABB + SAT 精确检测
// ============================================================================
bool checkCollision(ICollidable *objA, ICollidable *objB)
{
    if (!objA || !objB)
        return false;

    // 第一步：AABB 快速排除
    sf::FloatRect boundsA = objA->getGlobalBounds();
    sf::FloatRect boundsB = objB->getGlobalBounds();

    if (!aabbCollide(boundsA, boundsB))
        return false;

    // 第二步：判断是否需要 SAT 精确检测
    // 仅当碰撞涉及 Bullet 时才使用 SAT
    if (involvesBullet(objA->getType(), objB->getType()))
    {
        return satCollide(objA->getVertices(), objB->getVertices());
    }

    // Player-Monster：AABB 相交即认为碰撞
    return true;
}

// ============================================================================
// 批量碰撞检测
// 遍历所有对象对，返回发生碰撞的对象对列表（不重复）
// ============================================================================
std::vector<std::pair<ICollidable *, ICollidable *>> detectCollisions(const std::vector<ICollidable *> &objects)
{
    std::vector<std::pair<ICollidable *, ICollidable *>> result;

    for (size_t i = 0; i < objects.size(); ++i)
    {
        for (size_t j = i + 1; j < objects.size(); ++j)
        {
            if (checkCollision(objects[i], objects[j]))
            {
                result.emplace_back(objects[i], objects[j]);
            }
        }
    }

    return result;
}
