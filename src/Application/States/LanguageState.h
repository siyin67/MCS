#pragma once
#include "IState.h"
#include <SFML/Graphics.hpp>
#include <vector>

// 「语言」页（原版风格）：中间一列语言条目，选中项套一个深色框；
// 往 assets/lang 里丢新的 <code>.json 就会自动出现在列表里，
// 条目多了可以滚轮滚动，右侧有原版那样的滚动条。
class LanguageState : public IState
{
public:
    LanguageState();

    void handleInput(sf::RenderWindow &window) override;
    void handleEvent(const sf::Event &event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window) override;

private:
    static constexpr float LIST_W = 520.0f;
    static constexpr float ROW_H = 30.0f;
    static constexpr float LIST_TOP = 120.0f;
    static constexpr float LIST_BOTTOM_MARGIN = 150.0f;
    static constexpr float DONE_W = 400.0f;
    static constexpr float DONE_H = 50.0f;

    sf::RectangleShape dim_;
    sf::Font font_;
    sf::Text titleText_;
    sf::Text noteText_;

    std::vector<sf::RectangleShape> rowBoxes_;  // 选中 / 悬停的底色
    std::vector<sf::Text> rowTexts_;
    std::vector<sf::FloatRect> rowRects_;
    std::vector<std::string> codes_;

    sf::RectangleShape scrollTrack_;
    sf::RectangleShape scrollThumb_;

    sf::RectangleShape doneButton_;
    sf::Text doneText_;

    float viewTop_ = LIST_TOP;
    float viewBottom_ = 560.0f;
    float scrollOffset_ = 0.0f;
    int hoverIndex_ = -1;
    bool doneHover_ = false;
    bool mouseWasDown_ = true;
    bool escWasDown_ = true;
    int langVersion_ = -1;

    void rebuildRows();
    void applyLanguage();
    void updateLayout(sf::Vector2u winSize);
    float maxScroll() const;
    void clampScroll();
    int hitTestRow(sf::Vector2f mouse) const;
};
