#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Projectile.h"

class ProjectileManager
{
private:
    std::vector<std::unique_ptr<Projectile>> projectiles_;

public:
    ProjectileManager() = default;
    ~ProjectileManager() = default;
    
    // 禁止拷贝
    ProjectileManager(const ProjectileManager&) = delete;
    ProjectileManager& operator=(const ProjectileManager&) = delete;
    
    // 添加投射物
    void add(std::unique_ptr<Projectile> projectile);
    
    // 更新所有投射物
    void update(float deltaTime);
    
    // 渲染所有投射物
    void render(sf::RenderWindow& window);
    
    // 碰撞检测（对怪物列表）
    void checkCollisions(std::vector<std::unique_ptr<Monster>>& monsters);

    // 范围伤害检测（TNT等爆炸类投射物）
    void checkAreaDamage(std::vector<std::unique_ptr<Monster>>& monsters);

    // 追踪目标更新（三叉戟等追踪类投射物）
    void updateTracking(const sf::Vector2f &playerPos);

    // 弹射目标更新（海洋之心等弹射类投射物）
    void updateTargeting(const std::vector<std::unique_ptr<Monster>> &monsters);

    // 清理死亡的投射物
    void removeDead();
    
    // 获取投射物列表（用于其他碰撞）
    std::vector<std::unique_ptr<Projectile>>& getAll();
    
    // 清空
    void clear();
};