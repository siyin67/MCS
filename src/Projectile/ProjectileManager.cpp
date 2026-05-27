// src/Projectile/ProjectileManager.cpp
#include "ProjectileManager.h"
#include "Monster/Monster.h"
#include <algorithm>

void ProjectileManager::add(std::unique_ptr<Projectile> projectile)
{
    projectiles_.push_back(std::move(projectile));
}

void ProjectileManager::update(float deltaTime)
{
    for (auto &p : projectiles_)
    {
        p->update(deltaTime);
    }
}

void ProjectileManager::render(sf::RenderWindow &window)
{
    for (auto &p : projectiles_)
    {
        p->render(window);
    }
}

void ProjectileManager::checkCollisions(std::vector<std::unique_ptr<Monster>> &monsters)
{
    for (auto &p : projectiles_)
    {
        if (!p->isActive())
            continue;

        sf::FloatRect projBounds = p->getBounds();

        for (auto &m : monsters)
        {
            if (!m->isAlive())
                continue;

            if (projBounds.findIntersection(m->getBounds()).has_value())
            {
                p->onHit(*m);
                p->setActive(false);
                break;
            }
        }
    }
}

void ProjectileManager::checkAreaDamage(std::vector<std::unique_ptr<Monster>> &monsters)
{
	for (auto &p : projectiles_)
	{
		if (!p->isActive())
			continue;
		p->checkAreaDamage(monsters);
	}
}

void ProjectileManager::updateTracking(const sf::Vector2f &playerPos)
{
	for (auto &p : projectiles_)
	{
		if (!p->isActive())
			continue;
		p->setTrackingTarget(playerPos);
	}
}

void ProjectileManager::removeDead()
{
    projectiles_.erase(
        std::remove_if(projectiles_.begin(), projectiles_.end(),
                       [](const std::unique_ptr<Projectile> &p)
                       { return !p->isActive(); }),
        projectiles_.end());
}

std::vector<std::unique_ptr<Projectile>> &ProjectileManager::getAll()
{
    return projectiles_;
}

void ProjectileManager::clear()
{
    projectiles_.clear();
}