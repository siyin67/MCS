#pragma once
#include "IState.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Player;

class UpgradeState : public IState
{
private:
    Player *player_;
    int selectedIndex_;

    sf::RectangleShape bg_; // 半透明遮罩
    sf::Font font_;
    sf::Text titleText_;

    // 全部用 RectangleShape
    sf::RectangleShape panelBg_;   // 升级界面背景（包含三个按钮）
    sf::RectangleShape iconFrame_; // 图标框
    sf::RectangleShape icon_;      // 武器图标

public:
    UpgradeState(Player *player);
    ~UpgradeState() = default;

    void handleInput(sf::RenderWindow &window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window) override;

private:
    void applyOption(int index);
    void skip();
};