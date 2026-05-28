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
#include "Weapon/weapon.h"
#include "Weapon/ClockWeapon.h"
#include "Item/XpOrb.h"
#include "Item/PotionItem.h"
#include "Monster/Zombie.h"
#include "Monster/Skeleton.h"
#include "Monster/Spider.h"
#include "Monster/Drowned.h"
#include "Monster/Breeze.h"
#include "Monster/Creeper.h"
#include "Monster/Enderman.h"
#include "Monster/Witch.h"
#include "Monster/Blaze.h"
#include "Monster/WitherSkeleton.h"
#include "Monster/Ghast.h"
#include "Monster/Wither.h"
#include "Monster/Enderdragon.h"
#include "Monster/Slimes.h"
#include <random>

GameState::GameState() : mapWidth_(312), mapHeight_(312), tileSize_(16), mapSprite_(nullptr)
{
    std::cout << "GameState constructor START" << std::endl;
    player_.addWeapon(std::move(WeaponFactory::getInstance().create("sword"))); // 添加一把默认的剑

    // ClockWeapon needs WeaponManager pointer
    for (auto &w : player_.getWeaponManager().getWeapons())
    {
        if (auto *cw = dynamic_cast<ClockWeapon *>(w.get()))
            cw->setWeaponManager(&player_.getWeaponManager());
    }

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
    setupWeaponCallbacks();
    std::cout << "GameState constructor END" << std::endl;
}
void GameState::setupWeaponCallbacks()
{
    auto callback = [this](std::unique_ptr<Projectile> projectile)
    {
        projectileManager_.add(std::move(projectile));
    };

    Monster::spawnAreaCallback = [this](sf::Vector2f pos, float radius, float duration, BuffType type, int value)
    {
        AreaEffect area;
        area.position = pos;
        area.radius = radius;
        area.duration = duration;
        area.timer = duration;
        area.buffType = type;
        area.buffValue = value;
        switch (type)
        {
        case BuffType::Wither:
            area.color = sf::Color(80, 80, 80, 180);
            break;
        case BuffType::Burn:
            area.color = sf::Color(255, 100, 20, 180);
            break;
        case BuffType::Poison:
            area.color = sf::Color(160, 50, 200, 180);
            break;
        default:
            area.color = sf::Color(100, 100, 100, 180);
            break;
        }
        areaEffects_.push_back(area);
    };

    // 为所有武器设置回调（不需要判断类型）
    for (auto &weapon : player_.getWeaponManager().getWeapons())
    {
        weapon->setAddProjectileCallback(callback);
    }
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

    // 按空格回复血量
    bool spacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
    if (spacePressed && !wasPressed)
    {
        player_.heal(100);
    }

    // 按 M 一次性生成所有种类怪物（调试用）
    bool mPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::M);
    if (mPressed && !wasPressed)
    {
        std::cout << "[DEBUG] Spawning all monster types" << std::endl;
        sf::Vector2f pos = player_.getPosition();
        float r = 150.0f;
        auto spawn = [&](auto m)
        { m->setPosition(pos + sf::Vector2f(r, 0)); r += 80.0f; monsters_.push_back(std::move(m)); };
        spawn(std::make_unique<Enderdragon>());
        spawn(std::make_unique<Wither>());
        spawn(std::make_unique<Spider>());
        spawn(std::make_unique<Wither>());
        spawn(std::make_unique<Enderdragon>());
        spawn(std::make_unique<Creeper>());
        spawn(std::make_unique<Enderman>());
        spawn(std::make_unique<Witch>());
        spawn(std::make_unique<Blaze>());
        spawn(std::make_unique<WitherSkeleton>());
        spawn(std::make_unique<BigSlime>());
        spawn(std::make_unique<Ghast>());
    }

    wasPressed = escPressed || zeroPressed || ninePressed || spacePressed || mPressed;
}

void GameState::update(float deltaTime)
{
    for (auto &weapon : player_.getWeaponManager().getWeapons())
    {
        if (!weapon->hasCallback())
        {
            weapon->setAddProjectileCallback([this](auto p)
                                             { projectileManager_.add(std::move(p)); });
        }
    }
    // 更新玩家
    player_.update(deltaTime);
    player_.updateWeapons(deltaTime, player_.getPosition(), player_.getAttackDirection(), monsters_);
    player_.getWeaponManager().updateShields(deltaTime, player_.getPosition(), monsters_);
    spawner_.update(deltaTime, player_.getPosition(), monsters_);
    //  更新怪物
    for (auto &m : monsters_)
    {
        m->update(deltaTime, player_.getPosition(), monsters_);
    }
    // 更新投掷物
    projectileManager_.update(deltaTime);
    projectileManager_.updateTracking(player_.getPosition());
    projectileManager_.updateTargeting(monsters_);
    projectileManager_.checkAreaDamage(monsters_);
    // 碰撞检测
    std::vector<ICollidable *> collidables;
    collidables.push_back(&player_);
    for (auto &m : monsters_)
    {
        if (m->isAlive())
            collidables.push_back(m.get());
    }
    projectileManager_.checkCollisions(monsters_);
    projectileManager_.removeDead();
    // 更新掉落物
    dropItemManager_.update(deltaTime, player_.getPosition());
    int collected = dropItemManager_.checkPlayerCollision(player_);
    if (collected > 0)
    {
        int levelUps = player_.addXP(collected);
        if (levelUps > 0)
        {
            SoundManager::getInstance().playSFX("levelup");
            stateManager_->pushState(std::make_unique<UpgradeState>(&player_));
        }
    }
    dropItemManager_.removeDead();

    // 更新 UI
    uiManager_.update(player_, deltaTime);
    // 检查 BGM 循环
    SoundManager::getInstance().updateBGM();

    auto collisionPairs = detectCollisions(collidables);

    // 处理碰撞结果（玩家受伤、怪物受伤等）
    resolveCollisions(collisionPairs, player_);
    if (!player_.isAlive())
    {
        stateManager_->pushState(std::make_unique<GameOverState>());
    }
    // 清理死亡怪物（回调生成经验球 + 5%药水掉落）
    removeDeadMonsters(monsters_, [this](sf::Vector2f pos, int xp)
                       {
        dropItemManager_.add(std::make_unique<XpOrb>(pos, xp * 4));
        // 5% 概率掉药水
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> dis(0.0, 1.0);
        if (dis(gen) < 0.02f)
        {
            static const BuffType potionTypes[] = {
                BuffType::Heal, BuffType::SpeedUp, BuffType::AttackUp, BuffType::Invincible
            };
            static const float potionValues[] = {4.0f, 1.2f, 1.3f, 1.0f};
            static const float potionDurations[] = {0.0f, 8.0f, 8.0f, 6.0f};
            int idx = static_cast<int>(dis(gen) * 4) % 4;
            dropItemManager_.add(std::make_unique<PotionItem>(
                pos, potionTypes[idx], potionValues[idx], potionDurations[idx]));
        } });
    // 处理怪物技能产生的延迟生成
    for (auto &m : Monster::pendingSpawns)
        monsters_.push_back(std::move(m));
    Monster::pendingSpawns.clear();
    // 处理怪物对玩家的外力
    player_.applyExternalForce(Monster::pendingPlayerForce);
    Monster::pendingPlayerForce = {0.0f, 0.0f};
    // 处理怪物投掷物
    for (auto &p : Monster::pendingProjectiles)
        projectileManager_.add(std::move(p));
    Monster::pendingProjectiles.clear();
    // 怪物投掷物碰撞玩家 + 区域效果对玩家伤害
    checkProjectilePlayerCollisions();
    updateAreaEffects(deltaTime);

    // 岩浆地板
    for (auto &l : LavaProjectile::pendingFloors)
        lavaFloors_.push_back(l);
    LavaProjectile::pendingFloors.clear();
    updateLavaFloors(deltaTime);
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
    projectileManager_.render(window);
    renderAreaEffects(window);
    renderLavaFloors(window);
    player_.render(window);
    player_.getWeaponManager().render(window, player_.getPosition());
    dropItemManager_.render(window);
    // UI（屏幕坐标）
    sf::Vector2u winSize = window.getSize();
    window.setView(sf::View(sf::Vector2f(winSize.x / 2.f, winSize.y / 2.f), sf::Vector2f(winSize)));
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

void GameState::checkProjectilePlayerCollisions()
{
    for (auto &p : projectileManager_.getAll())
    {
        if (!p->isActive() || p->isFriendly())
            continue;
        if (p->getGlobalBounds().findIntersection(player_.getBounds()).has_value())
        {
            player_.takeDamage(p->getDamage());
            // 应用投掷物携带的 Buff
            const auto &hb = p->getHitBuff();
            if (hb.duration > 0.0f)
                player_.getBuffManager().add(hb.type, hb.value, hb.duration);
            p->setActive(false);
        }
    }
}

void GameState::updateAreaEffects(float deltaTime)
{
    for (auto &area : areaEffects_)
    {
        area.timer -= deltaTime;
        if (area.timer <= 0.0f)
            continue;

        // 检查玩家是否在区域内
        sf::Vector2f delta = player_.getPosition() - area.position;
        float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        if (dist <= area.radius)
        {
            // 刷新 Buff：玩家在区域内时保持 Buff 激活
            player_.getBuffManager().add(area.buffType, static_cast<float>(area.buffValue), 0.6f);
        }
    }

    // 清理过期区域效果
    areaEffects_.erase(
        std::remove_if(areaEffects_.begin(), areaEffects_.end(),
                       [](const AreaEffect &a)
                       { return a.timer <= 0.0f; }),
        areaEffects_.end());
}

void GameState::renderAreaEffects(sf::RenderWindow &window)
{
    for (const auto &area : areaEffects_)
    {
        if (area.timer <= 0.0f)
            continue;
        sf::CircleShape circle(area.radius);
        circle.setPosition(area.position - sf::Vector2f(area.radius, area.radius));
        circle.setFillColor(sf::Color::Transparent);
        circle.setOutlineColor(area.color);
        circle.setOutlineThickness(3.0f);
        // 透明度随剩余时间衰减
        float alpha = std::min(1.0f, area.timer / 0.5f);
        sf::Color outlineColor = area.color;
        outlineColor.a = static_cast<uint8_t>(255 * alpha);
        circle.setOutlineColor(outlineColor);
        window.draw(circle);
    }
}

void GameState::updateLavaFloors(float deltaTime)
{
    for (auto &l : lavaFloors_)
    {
        l.timer -= deltaTime;
        if (l.timer <= 0.0f)
            continue;
        sf::FloatRect lavaRect(l.position - sf::Vector2f(24.0f, 24.0f), {48.0f, 48.0f});
        for (auto &m : monsters_)
        {
            if (!m->isAlive())
                continue;
            if (lavaRect.findIntersection(m->getBounds()).has_value())
                m->getBuffManager().add(BuffType::Burn, static_cast<float>(l.dps), 0.6f);
        }
    }
    lavaFloors_.erase(
        std::remove_if(lavaFloors_.begin(), lavaFloors_.end(),
                       [](const LavaFloor &l)
                       { return l.timer <= 0.0f; }),
        lavaFloors_.end());
}

void GameState::renderLavaFloors(sf::RenderWindow &window)
{
    for (const auto &l : lavaFloors_)
    {
        sf::RectangleShape tile({48.0f, 48.0f});
        tile.setPosition(l.position - sf::Vector2f(24.0f, 24.0f));
        if (ResourceManager::GetInstance().HasTexture("lava"))
        {
            tile.setTexture(&ResourceManager::GetInstance().GetTexture("lava"));
            tile.setFillColor(sf::Color(255, 200, 100, 200));
        }
        else
        {
            tile.setFillColor(sf::Color(255, 100, 30, 200));
        }
        window.draw(tile);
    }
}
