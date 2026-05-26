#pragma once
#include <vector>
#include <memory>
#include "Weapon.h"
#include <SFML/Graphics.hpp>

class WeaponManager
{
private:
    std::vector<std::unique_ptr<Weapon>> weapons_;
    int maxWeapons_ = 6;

public:
    void addWeapon(std::unique_ptr<Weapon> weapon);
    void upgradeWeapon(int index);
    void update(float deltaTime, const sf::Vector2f &playerPos,
                const sf::Vector2f &attackDir,
                std::vector<std::unique_ptr<Monster>> &monsters);
    void render(sf::RenderWindow &window, const sf::Vector2f &playerPos) const; // 新增

    std::vector<std::unique_ptr<Weapon>> &getWeapons() { return weapons_; }
    const std::vector<std::unique_ptr<Weapon>> &getWeapons() const { return weapons_; }
    int getWeaponCount() const { return weapons_.size(); }
    void clear();
};