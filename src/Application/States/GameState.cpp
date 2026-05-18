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
#include <random>

GameState::GameState() : mapWidth_(312), mapHeight_(312), tileSize_(16), mapSprite_(nullptr)
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
    // 地图初始化
    generateMap();
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
    // 更新攻击方向
    // player_.updateAttackDirection(*window_);
    // Spawner 自动管理波次和生成
    spawner_.update(deltaTime, player_.getPosition(), monsters_);

    // 更新怪物
    for (auto &m : monsters_)
    {
        m->update(deltaTime, player_.getPosition(), monsters_);
    }
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
    player_.updateAttackDirection(window);
    window.setView(camera_);
    window.draw(background_);
    if (mapSprite_)
    {
        window.draw(*mapSprite_);
    }
    for (auto &m : monsters_)
        m->render(window);
    player_.render(window);
}

void GameState::generateMap()
{
    // 获取纹理
    sf::Texture &grassTex = ResourceManager::GetInstance().GetTexture("grass");
    sf::Texture &dirtTex = ResourceManager::GetInstance().GetTexture("dirt");
    sf::Texture &stoneTex = ResourceManager::GetInstance().GetTexture("stone");
    sf::Texture &cobbleTex = ResourceManager::GetInstance().GetTexture("cobblestone");
    sf::Texture &pathTex = ResourceManager::GetInstance().GetTexture("path");
    sf::Texture &waterTex = ResourceManager::GetInstance().GetTexture("water");
    sf::Texture &lavaTex = ResourceManager::GetInstance().GetTexture("lava");

    unsigned int w = static_cast<unsigned int>(mapWidth_ * tileSize_);
    unsigned int h = static_cast<unsigned int>(mapHeight_ * tileSize_);

    sf::RenderTexture renderTexture({w, h});
    renderTexture.clear(sf::Color::Transparent);

    for (int x = 0; x < mapWidth_; x++)
    {
        for (int y = 0; y < mapHeight_; y++)
        {
            int tileType = getTileType(x, y);

            sf::Texture *tex = nullptr;
            sf::Color color = sf::Color::White; // 其他纹理不叠加颜色

            switch (tileType)
            {
            case 0:
                tex = &grassTex;
                color = sf::Color(109, 153, 48); // 草地叠加绿色
                break;
            case 1:
                tex = &dirtTex;
                break;
            case 2:
                tex = &stoneTex;
                break;
            case 3:
                tex = &cobbleTex;
                break;
            case 4:
                tex = &pathTex;
                break;
            case 5:
                tex = &waterTex;
                color = sf::Color(63, 118, 228); // 水叠加蓝色
                break;
            case 6:
                tex = &lavaTex;
                break;
            default:
                tex = &grassTex;
                color = sf::Color(34, 139, 34);
                break;
            }

            sf::Sprite tile(*tex);
            tile.setColor(color);
            tile.setPosition({static_cast<float>(x * tileSize_),
                              static_cast<float>(y * tileSize_)});
            renderTexture.draw(tile);
        }
    }

    renderTexture.display();
    mapTexture_ = renderTexture.getTexture();
    mapSprite_ = std::make_unique<sf::Sprite>(mapTexture_);
}

int GameState::getTileType(int x, int y)
{
    // 边界为水
    if (x == 0 || y == 0 || x == mapWidth_ - 1 || y == mapHeight_ - 1)
    {
        return 5;
    }

    float height = getHeight(x, y);
    float moisture = getMoisture(x, y);

    // 水
    if (height < -0.3f)
        return 5;
    if (height < -0.2f)
        return 4; // 沙滩

    // 草地（分湿/干）
    if (height < 0.1f)
        return 0;
    if (height < 0.2f)
        return (moisture > 0.5f) ? 0 : 4;
    if (height < 0.3f)
        return 0;
    if (height < 0.4f)
        return 1; // 泥土
    if (height < 0.5f)
        return 2; // 石头
    if (height < 0.6f)
        return (moisture > 0.5f) ? 2 : 3;

    return 3; // 圆石（山顶）
}
float GameState::fractalNoise(float x, float y, int octaves)
{
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 0.05f; // 提高频率，地形变化更快

    for (int i = 0; i < octaves; i++)
    {
        value += amplitude * std::sin(x * frequency) * std::cos(y * frequency);
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }

    return value;
}
float GameState::getHeight(int x, int y)
{
    float fx = static_cast<float>(x);
    float fy = static_cast<float>(y);

    // 主地形 + 细节扰动
    float mainHeight = fractalNoise(fx, fy, 3);                  // 主要地形
    float detail = fractalNoise(fx * 3.0f, fy * 3.0f, 2) * 0.3f; // 细节

    float height = mainHeight * 0.7f + detail * 0.3f;

    return height;
}
float GameState::getMoisture(int x, int y)
{
    float fx = static_cast<float>(x);
    float fy = static_cast<float>(y);

    // 湿度也需要更高的频率
    float moisture = fractalNoise(fx + 1000.0f, fy + 1000.0f, 3);
    moisture = (moisture + 1.0f) / 2.0f;

    // 添加小尺度变化
    float micro = fractalNoise(fx * 5.0f, fy * 5.0f, 1) * 0.2f;
    moisture += micro;
    moisture = std::max(0.0f, std::min(1.0f, moisture));

    return moisture;
}
