#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Player;
class Weapon;

class UIManager
{
private:
    // 红心
    int fullHearts_ = 10;
    int halfHearts_ = 0;
    int emptyHearts_ = 0;

    // 物品栏
    std::unique_ptr<sf::Sprite> hotbarSprite_;
    float slotWidth_ = 0.0f;
    float slotHeight_ = 0.0f;

    // 字体
    sf::Font font_;

    // 武器信息文字
    sf::Text weaponInfoText_;

    // 当前持有的武器列表
    std::vector<Weapon*> weapons_;

    static constexpr int MAX_HEARTS = 10;
    static constexpr float HEART_SPACING = 28.0f;
    static constexpr int HOTBAR_SLOTS = 9;

public:
    UIManager();

    void update(const Player& player, const std::vector<Weapon*>& weapons);
    void render(sf::RenderWindow& window);
};
