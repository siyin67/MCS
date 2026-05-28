#pragma once
#include "IState.h"
#include <SFML/Graphics.hpp>
#include "Player/player.h"
#include "Monster/Monster.h"
#include "Monster/Spawner.h"
#include "UI/UIManager.h"
#include "../../Weapon/weapon.h"
#include "Projectile/ProjectileManager.h"
#include "Projectile/MonsterProjectile.h"
#include "Item/DropItemManager.h"
#include <memory>

struct AreaEffect
{
    sf::Vector2f position;
    float radius;
    float duration;
    float timer;
    int dps;
    float damageAccum;
    sf::Color color;
    MonsterProjectile::AreaType type;
};

class GameState : public IState
{
private:
    sf::RectangleShape background_;
    sf::View camera_;
    Player player_;
    std::vector<std::unique_ptr<Monster>> monsters_;
    Spawner spawner_;
    UIManager uiManager_;
    ProjectileManager projectileManager_;
    DropItemManager dropItemManager_;
    std::vector<AreaEffect> areaEffects_;

    sf::Texture mapTexture_;
    std::unique_ptr<sf::Sprite> mapSprite_;
    int mapWidth_;
    int mapHeight_;
    int tileSize_;

    void generateMap();
    float fractalNoise(float x, float y, int octaves);
    float getHeight(int x, int y);
    float getMoisture(int x, int y);
    int getTileType(int x, int y);
    void updateAreaEffects(float deltaTime);
    void renderAreaEffects(sf::RenderWindow &window);
    void checkProjectilePlayerCollisions();

public:
    GameState();
    void handleInput(sf::RenderWindow &window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window) override;
    void setupWeaponCallbacks();
};