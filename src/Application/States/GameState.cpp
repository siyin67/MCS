// GameState.cpp
#include "GameState.h"
#include "MenuState.h"
#include "PauseState.h"
#include "UpgradeState.h"
#include "GameOverState.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "../Statemanager.h"
#include "Collision/Collision.h"
#include "Collision/CollisionResolver.h"
#include "Weapon/WeaponFactory.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include "Monster/Zombie.h"
#include "Weapon/weapon.h"
#include <random>

GameState::GameState() : mapWidth_(312), mapHeight_(312), tileSize_(16), mapSprite_(nullptr)
{
    std::cout << "GameState constructor START" << std::endl;
    player_.addWeapon(std::move(WeaponFactory::getInstance().create("sword")));
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
    // bgm初始化
    std::vector<std::string> playlist = {
        "assets/bgm/Anthem of the Savannah.mp3",
        "assets/bgm/Lamentation et triomphe.mp3",
        "assets/bgm/To Light the Aeon Dark.mp3"};
    SoundManager::getInstance().setSFXVolume(5);
    SoundManager::getInstance().setBGMPlaylist(playlist);
    SoundManager::getInstance().playRandomBGM();
    SoundManager::getInstance().setBGMVolume(5);
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

    // 按数字 9 触发升级界面
    bool ninePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num9);
    if (ninePressed && !wasPressed)
    {
        std::cout << "Upgrade triggered by 9 key" << std::endl;
        stateManager_->pushState(std::make_unique<UpgradeState>(&player_));
    }

    wasPressed = escPressed || zeroPressed || ninePressed;
}

void GameState::update(float deltaTime)
{
    // 更新玩家
    player_.update(deltaTime);
    player_.updateWeapons(deltaTime, player_.getPosition(), player_.getAttackDirection(), monsters_);
    spawner_.update(deltaTime, player_.getPosition(), monsters_);
    // 更新怪物
    for (auto &m : monsters_)
    {
        m->update(deltaTime, player_.getPosition(), monsters_);
    }
    // 碰撞检测
    std::vector<ICollidable *> collidables;
    collidables.push_back(&player_);
    for (auto &m : monsters_)
    {
        if (m->isAlive())
            collidables.push_back(m.get());
    }
    // 更新 UI
    std::vector<Weapon *> weaponPtrs;
    for (auto &w : weapons_)
        weaponPtrs.push_back(w.get());
    uiManager_.update(player_, weaponPtrs);

    auto collisionPairs = detectCollisions(collidables);

    // 处理碰撞结果（玩家受伤、怪物受伤等）
    resolveCollisions(collisionPairs, player_);
    if (!player_.isAlive())
    {
        stateManager_->pushState(std::make_unique<GameOverState>());
    }
    // 清理死亡怪物
    removeDeadMonsters(monsters_);
    // 更新相机
    camera_.setCenter(player_.getPosition());
}

void GameState::render(sf::RenderWindow &window)
{
    window.setView(camera_);
    player_.updateAttackDirection(window);
    window.draw(background_);
    if (mapSprite_)
    {
        window.draw(*mapSprite_);
    }
    for (auto &m : monsters_)
        m->render(window);
    player_.render(window);

    // UI（屏幕坐标）
    window.setView(window.getDefaultView());
    uiManager_.render(window);
}
class PerlinNoise
{
private:
    static float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
    static float lerp(float a, float b, float t) { return a + t * (b - a); }
    static float grad(int hash, float x, float y)
    {
        int h = hash & 7;
        float u = h < 4 ? x : y;
        float v = h < 4 ? y : x;
        return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
    }

    static int hash(int x, int y)
    {
        unsigned int n = x * 374761393 + y * 668265263;
        n = (n ^ (n >> 13)) * 1274126177;
        return (n ^ (n >> 15)) & 255;
    }

public:
    static float noise(float x, float y)
    {
        int xi = (int)std::floor(x) & 255;
        int yi = (int)std::floor(y) & 255;
        float fx = x - std::floor(x);
        float fy = y - std::floor(y);

        float u = fade(fx);
        float v = fade(fy);

        int aa = hash(xi, yi);
        int ab = hash(xi, yi + 1);
        int ba = hash(xi + 1, yi);
        int bb = hash(xi + 1, yi + 1);

        float a = lerp(grad(aa, fx, fy), grad(ba, fx - 1, fy), u);
        float b = lerp(grad(ab, fx, fy - 1), grad(bb, fx - 1, fy - 1), u);

        return (lerp(a, b, v) + 1.0f) / 2.0f; // 返回 0-1
    }
};

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
    if (height < -0.4f)
        return 5;
    if (height < -0.25f)
        return 4; // 沙滩

    // 草地（分湿/干）
    if (height < 0.1f)
        return 0;
    if (height < 0.2f)
        return (moisture > 0.5f) ? 0 : 4;
    if (height < 0.35f)
        return 0;
    if (height < 0.5f)
        return 1; // 泥土
    if (height < 0.65f)
        return 2; // 石头
    if (height < 0.8f)
        return (moisture > 0.5f) ? 2 : 3;

    return 3; // 圆石（山顶）
}
float GameState::fractalNoise(float x, float y, int octaves)
{
    float value = 0.0f;
    float amplitude = 1.0f;
    float persistence = 0.5f;
    float frequency = 0.03f; // 添加频率

    for (int i = 0; i < octaves; i++)
    {
        value += amplitude * (PerlinNoise::noise(x * frequency, y * frequency) * 2.0f - 1.0f);
        amplitude *= persistence;
        frequency *= 2.0f;
    }

    return value;
}
float GameState::getHeight(int x, int y)
{
    float fx = static_cast<float>(x);
    float fy = static_cast<float>(y);

    // 主地形 + 细节扰动
    float mainHeight = fractalNoise(fx * 0.3f, fy * 0.3f, 4);
    float detail = fractalNoise(fx * 1.2f, fy * 1.2f, 2) * 0.2f;

    float height = mainHeight * 0.8f + detail;

    // 扩大范围让地形变化更明显
    height = height * 1.2f;

    return std::max(-1.0f, std::min(1.0f, height));
}
float GameState::getMoisture(int x, int y)
{
    float fx = static_cast<float>(x);
    float fy = static_cast<float>(y);

    // 使用不同的频率和偏移
    float moisture = fractalNoise(fx * 0.15f + 500.0f, fy * 0.15f + 500.0f, 3);
    moisture = (moisture + 1.0f) / 2.0f;

    // 添加小尺度变化
    float micro = fractalNoise(fx * 0.6f, fy * 0.6f, 1) * 0.15f;
    moisture += micro;
    moisture = std::max(0.0f, std::min(1.0f, moisture));

    return moisture;
}
