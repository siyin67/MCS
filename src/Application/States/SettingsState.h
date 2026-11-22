#pragma once
#include "IState.h"
#include <SFML/Graphics.hpp>
#include <vector>

// 「选项」总设置页（原版二级菜单的父级）：
// 目前只有游戏里真实存在的两项 —— 音乐和声音 / 语言，
// 以后加视频、按键等设置就在 kItems 里加一行。
class SettingsState : public IState
{
public:
    SettingsState();

    void handleInput(sf::RenderWindow &window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window) override;

private:
    static constexpr int ITEM_COUNT = 2; // 音乐和声音 / 语言

    sf::RectangleShape dim_;
    sf::Font font_;
    sf::Text titleText_;

    std::vector<sf::RectangleShape> buttons_;
    std::vector<sf::Text> texts_;
    std::vector<sf::FloatRect> rects_;

    sf::RectangleShape doneButton_;
    sf::Text doneText_;

    int hoverIndex_ = -1;
    bool doneHover_ = false;
    bool mouseWasDown_ = true;
    bool escWasDown_ = true;
    int langVersion_ = -1;

    void updateLayout(sf::Vector2u winSize);
    void applyLanguage();
    void openItem(int index);
    static const char *itemKey(int index);
};
