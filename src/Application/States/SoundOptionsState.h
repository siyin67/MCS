#pragma once
#include "IState.h"
#include <SFML/Graphics.hpp>
#include <vector>

// 「音乐和声音选项」（原版风格，叠加层）：
// 压在上层界面之上，StateManager 会把整栈依次渲染，所以背景会透出来。
// 每一行就是一条横贯的滑条，文字居中画在滑条上（"主音量: 100%"），
// 在滑条上任意位置按下即可跳到该值并继续左右拖动；底部一个"完成"返回。
class SoundOptionsState : public IState
{
public:
    SoundOptionsState();

    void handleInput(sf::RenderWindow &window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window) override;

private:
    static constexpr int ROW_COUNT = 5; // 主音量 / 武器 / 怪物 / 音乐 / 音效

    sf::RectangleShape dim_;
    sf::Font font_;
    sf::Text titleText_;

    std::vector<sf::RectangleShape> bars_;    // 滑条底（slider_track）
    std::vector<sf::RectangleShape> handles_; // 滑块（slider_handle）
    std::vector<sf::Text> texts_;             // "主音量: 100%"
    std::vector<sf::FloatRect> rowRects_;     // 每行的可点击 / 可拖动区域

    sf::RectangleShape doneButton_;
    sf::Text doneText_;

    int draggingRow_ = -1;
    int hoverRow_ = -1;
    bool doneHover_ = false;
    bool mouseWasDown_ = true; // 进入界面的那一帧鼠标还按着，先等它松开
    bool escWasDown_ = true;
    int langVersion_ = -1; // 语言变了就重取文字

    void updateLayout(sf::Vector2u winSize);
    void applyLanguage();
    void syncRowText(int row);
    void syncAllTexts();
    int hitTestRow(sf::Vector2f mouse) const;
    int getRowVolume(int row) const;
    void applyRowVolume(int row, int volume);
    void playPreviewSound(int row);
    static const char *rowKey(int row);
};
