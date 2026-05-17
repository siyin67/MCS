// GameState.cpp
#include "GameState.h"
#include "MenuState.h"
#include "PauseState.h"
#include "GameOverState.h"
#include "Resourcemanager/ResourceManager.h"
#include "../Statemanager.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include "Monster/Zombie.h"

GameState::GameState()
{
    std::cout << "GameState constructor START" << std::endl;
    // 怪物类注册
    spawner_.registerMonster([]()
                             { return std::make_unique<Zombie>(); });
    // 纹理初始化
    if (ResourceManager::GetInstance().HasTexture("map1"))
    {
        sf::Texture &bgTex = ResourceManager::GetInstance().GetTexture("map1");
        background_.setTexture(&bgTex);
    }
    // background_.setFillColor(sf::Color::Blue);
    background_.setSize({5000, 5000});
    background_.setPosition({0, 0});
    // 相机初始化
    camera_.setCenter(player_.getPosition());
    camera_.setSize({1920, 1080});
    std::cout << "GameState constructor END" << std::endl;
}

void GameState::handleInput(sf::RenderWindow &window)
{
    static bool wasPressed = false;

    // 按 ESC 暂停
    bool escPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
    if (escPressed && !wasPressed)
    {
        std::cout << "Game paused" << std::endl;
        stateManager_->pushState(std::make_unique<PauseState>());
    }

    // 按数字 0 触发游戏结束
    bool zeroPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num0);
    if (zeroPressed && !wasPressed)
    {
        std::cout << "Game over triggered by 0 key" << std::endl;
        stateManager_->pushState(std::make_unique<GameOverState>());
    }

    wasPressed = escPressed || zeroPressed;
}

void GameState::update(float deltaTime)
{
    // 更新玩家
    player_.update(deltaTime);
    // Spawner 自动管理波次和生成
    spawner_.update(deltaTime, player_.getPosition(), monsters_);

    // 更新怪物
    for (auto &m : monsters_)
        m->update(deltaTime, player_.getPosition());
    // 碰撞检测（暂无）
    // 删除死亡怪物
    for (auto it = monsters_.begin(); it != monsters_.end();)
    {
        if (!(*it)->isAlive())
        {
            // 掉落经验球等
            // player_.addExp((*it)->getExpReward());
            it = monsters_.erase(it);
        }
        else
        {
            ++it;
        }
    }
    // 更新相机
    camera_.setCenter(player_.getPosition());
}

void GameState::render(sf::RenderWindow &window)
{
    window.setView(camera_);
    window.draw(background_);
    for (auto &m : monsters_)
        m->render(window);
    player_.render(window);
}