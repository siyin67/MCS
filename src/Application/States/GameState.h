#pragma once
#include "IState.h"
#include <SFML/Graphics.hpp>
#include "Player/player.h"
#include "Monster/Monster.h"
#include "Monster/Spawner.h"
#include "UI/UIManager.h"
#include "../../Weapon/weapon.h"
#include <memory>

class GameState : public IState
{
private:
    sf::RectangleShape background_;
    sf::View camera_;
    Player player_;
    std::vector<std::unique_ptr<Monster>> monsters_;
    Spawner spawner_;
    UIManager uiManager_;
    std::vector<std::unique_ptr<Weapon>> weapons_;

    sf::Texture mapTexture_;
    std::unique_ptr<sf::Sprite> mapSprite_; // 改为指针
    int mapWidth_;                          // 瓦片数量
    int mapHeight_;                         // 瓦片数量
    int tileSize_;                          // 16 像素

    void generateMap();
    float fractalNoise(float x, float y, int octaves);
    float getHeight(int x, int y);
    float getMoisture(int x, int y);
    int getTileType(int x, int y);

public:
    GameState();
    void handleInput(sf::RenderWindow &window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window) override;
};