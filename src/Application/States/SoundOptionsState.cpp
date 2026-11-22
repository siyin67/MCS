#include "SoundOptionsState.h"
#include "../Statemanager.h"
#include "Localization/Localization.h"
#include "Localization/UiText.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

namespace
{
// 原版排版：主音量独占整行，其余 2x2 排列，底部一条同宽按钮
constexpr float BAR_W = 400.0f;                // 半行滑条宽
constexpr float BAR_H = 40.0f;                 // 滑条高
constexpr float GAP_X = 40.0f;                 // 两列间距
constexpr float FULL_W = BAR_W * 2.0f + GAP_X; // 整行宽（840）
constexpr float TOP_Y = 170.0f;                // 第一条滑条的 y
constexpr float ROW_GAP = 72.0f;               // 行间距
constexpr float HANDLE_W = 12.0f;              // 滑块宽（高度跟滑条一样）
constexpr float DONE_H = 50.0f;
constexpr float DONE_MARGIN_BOTTOM = 60.0f;

// 把文字放到矩形正中（考虑字形包围盒的偏移）
void centerInRect(sf::Text &text, const sf::FloatRect &rect)
{
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setPosition({rect.position.x + (rect.size.x - bounds.size.x) / 2.0f - bounds.position.x,
                      rect.position.y + (rect.size.y - bounds.size.y) / 2.0f - bounds.position.y});
}
} // namespace

const char *SoundOptionsState::rowKey(int row)
{
    switch (row)
    {
    case 0:
        return "sound.master";
    case 1:
        return "sound.weapon";
    case 2:
        return "sound.monster";
    case 3:
        return "sound.bgm";
    default:
        return "sound.sfx";
    }
}

SoundOptionsState::SoundOptionsState()
    : font_("assets/fonts/minecraft.ttf"), titleText_(font_), doneText_(font_)
{
    std::cout << "SoundOptionsState created" << std::endl;

    dim_.setFillColor(sf::Color(12, 12, 14, 255)); // 二级页用不透明底，避免上一级页面的按钮透出来

    titleText_.setCharacterSize(26);
    titleText_.setFillColor(sf::Color::White);
    titleText_.setOutlineColor(sf::Color(25, 25, 25));
    titleText_.setOutlineThickness(1.0f);

    ResourceManager &rm = ResourceManager::GetInstance();
    const bool hasTrack = rm.HasTexture("slider_track");
    const bool hasHandle = rm.HasTexture("slider_handle");

    bars_.resize(ROW_COUNT);
    handles_.resize(ROW_COUNT);
    rowRects_.resize(ROW_COUNT);
    texts_.reserve(ROW_COUNT);

    for (int i = 0; i < ROW_COUNT; ++i)
    {
        if (hasTrack)
            bars_[i].setTexture(&rm.GetTexture("slider_track"));
        else
            bars_[i].setFillColor(sf::Color(55, 55, 55));
        bars_[i].setFillColor(sf::Color::White); // 有贴图时白色 = 原色
        bars_[i].setOutlineColor(sf::Color::White);

        if (hasHandle)
            handles_[i].setTexture(&rm.GetTexture("slider_handle"));
        else
            handles_[i].setFillColor(sf::Color(230, 230, 230));
        handles_[i].setFillColor(sf::Color::White);

        texts_.emplace_back(font_);
        texts_.back().setCharacterSize(22);
        texts_.back().setFillColor(sf::Color::White);
        texts_.back().setOutlineColor(sf::Color(25, 25, 25));
        texts_.back().setOutlineThickness(1.0f);
    }

    if (rm.HasTexture("menu_button"))
        doneButton_.setTexture(&rm.GetTexture("menu_button"));
    else
        doneButton_.setFillColor(sf::Color(130, 130, 130));
    doneButton_.setFillColor(sf::Color::White);
    doneButton_.setSize({FULL_W, DONE_H});
    doneButton_.setOutlineColor(sf::Color::White);

    doneText_.setCharacterSize(24);
    doneText_.setFillColor(sf::Color::White);
    doneText_.setOutlineColor(sf::Color(25, 25, 25));
    doneText_.setOutlineThickness(1.0f);

    applyLanguage();
}

void SoundOptionsState::applyLanguage()
{
    langVersion_ = Localization::getInstance().version();
    ui_text::set(titleText_, tr("sound.title"));
    ui_text::set(doneText_, tr("sound.done"));
    syncAllTexts();
}

void SoundOptionsState::updateLayout(sf::Vector2u winSize)
{
    const float cx = winSize.x / 2.0f;
    const float left = cx - FULL_W / 2.0f;
    const float right = left + BAR_W + GAP_X;

    dim_.setSize({static_cast<float>(winSize.x), static_cast<float>(winSize.y)});
    dim_.setPosition({0.0f, 0.0f});

    titleText_.setPosition({cx - titleText_.getLocalBounds().size.x / 2.0f, 54.0f});

    for (int i = 0; i < ROW_COUNT; ++i)
    {
        float x = left;
        float y = TOP_Y;
        float w = FULL_W;

        if (i >= 1)
        {
            const int row = (i - 1) / 2; // 0 或 1
            y = TOP_Y + ROW_GAP * static_cast<float>(row + 1);
            w = BAR_W;
            x = ((i - 1) % 2 == 0) ? left : right;
        }

        rowRects_[i] = sf::FloatRect({x, y}, {w, BAR_H});

        bars_[i].setSize({w, BAR_H});
        bars_[i].setPosition({x, y});
        bars_[i].setOutlineThickness((hoverRow_ == i || draggingRow_ == i) ? 2.0f : 0.0f);

        const float ratio = static_cast<float>(getRowVolume(i)) / 100.0f;
        const float handleX = x + ratio * (w - HANDLE_W);
        handles_[i].setSize({HANDLE_W, BAR_H}); // 与滑条框同高
        handles_[i].setPosition({handleX, y});

        centerInRect(texts_[i], rowRects_[i]);
    }

    doneButton_.setPosition({left, static_cast<float>(winSize.y) - DONE_H - DONE_MARGIN_BOTTOM});
    doneButton_.setOutlineThickness(doneHover_ ? 2.0f : 0.0f);
    centerInRect(doneText_, doneButton_.getGlobalBounds());
}

void SoundOptionsState::handleInput(sf::RenderWindow &window)
{
    const sf::Vector2u winSize = window.getSize();
    window.setView(sf::View(sf::Vector2f(winSize.x / 2.f, winSize.y / 2.f), sf::Vector2f(winSize)));

    const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    const sf::Vector2f mouseCoords = window.mapPixelToCoords(mousePos);
    const bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    hoverRow_ = hitTestRow(mouseCoords);
    doneHover_ = doneButton_.getGlobalBounds().contains(mouseCoords);

    const bool mousePressed = mouseDown && !mouseWasDown_;
    const bool mouseReleased = !mouseDown && mouseWasDown_;
    mouseWasDown_ = mouseDown;

    if (mousePressed)
    {
        if (doneButton_.getGlobalBounds().contains(mouseCoords))
        {
            SoundManager::getInstance().playSFX("ui_click");
            stateManager_->popState();
            return;
        }
        draggingRow_ = hoverRow_; // 在滑条上按下即跳值（原版行为）
    }

    if (draggingRow_ >= 0 && mouseDown)
    {
        const sf::FloatRect &rect = rowRects_[draggingRow_];
        const float ratio = (mouseCoords.x - rect.position.x) / rect.size.x;
        const int volume = static_cast<int>(std::lround(std::clamp(ratio, 0.0f, 1.0f) * 20.0f)) * 5;
        applyRowVolume(draggingRow_, volume);
    }

    if (mouseReleased && draggingRow_ >= 0)
    {
        playPreviewSound(draggingRow_);
        draggingRow_ = -1;
    }

    const bool escPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
    if (escPressed && !escWasDown_)
    {
        SoundManager::getInstance().playSFX("ui_click");
        stateManager_->popState();
        return;
    }
    escWasDown_ = escPressed;

    // 输入处理完再排版：滑块当帧就跟上新的数值
    updateLayout(winSize);
}

void SoundOptionsState::update(float deltaTime)
{
    (void)deltaTime;
}

void SoundOptionsState::render(sf::RenderWindow &window)
{
    if (langVersion_ != Localization::getInstance().version())
    {
        applyLanguage();
        updateLayout(window.getSize());
    }

    window.draw(dim_);
    window.draw(titleText_);

    for (int i = 0; i < ROW_COUNT; ++i)
    {
        window.draw(bars_[i]);
        window.draw(handles_[i]);
        window.draw(texts_[i]);
    }

    window.draw(doneButton_);
    window.draw(doneText_);
}

int SoundOptionsState::hitTestRow(sf::Vector2f mouse) const
{
    for (int i = 0; i < ROW_COUNT; ++i)
    {
        if (rowRects_[i].contains(mouse))
        {
            return i;
        }
    }
    return -1;
}

int SoundOptionsState::getRowVolume(int row) const
{
    const SoundManager &sm = SoundManager::getInstance();
    switch (row)
    {
    case 0:
        return static_cast<int>(std::lround(sm.getMasterVolume()));
    case 1:
        return static_cast<int>(std::lround(sm.getWeaponVolume()));
    case 2:
        return static_cast<int>(std::lround(sm.getMonsterVolume()));
    case 3:
        return static_cast<int>(std::lround(sm.getBGMVolume()));
    case 4:
        return static_cast<int>(std::lround(sm.getSFXVolume()));
    default:
        return 0;
    }
}

void SoundOptionsState::applyRowVolume(int row, int volume)
{
    volume = std::clamp(volume, 0, 100);
    if (getRowVolume(row) == volume)
    {
        return; // 值没变就不动，避免拖动时反复写 settings.json
    }

    SoundManager &sm = SoundManager::getInstance();
    switch (row)
    {
    case 0:
        sm.setMasterVolume(static_cast<float>(volume));
        break;
    case 1:
        sm.setWeaponVolume(static_cast<float>(volume));
        break;
    case 2:
        sm.setMonsterVolume(static_cast<float>(volume));
        break;
    case 3:
        sm.setBGMVolume(static_cast<float>(volume));
        break;
    case 4:
        sm.setSFXVolume(static_cast<float>(volume));
        break;
    default:
        break;
    }

    syncRowText(row);
}

void SoundOptionsState::syncRowText(int row)
{
    ui_text::set(texts_[row], tr(rowKey(row)) + ": " + std::to_string(getRowVolume(row)) + "%");
    centerInRect(texts_[row], rowRects_[row]);
}

void SoundOptionsState::syncAllTexts()
{
    for (int i = 0; i < ROW_COUNT; ++i)
    {
        ui_text::set(texts_[i], tr(rowKey(i)) + ": " + std::to_string(getRowVolume(i)) + "%");
    }
}

void SoundOptionsState::playPreviewSound(int row)
{
    SoundManager &sm = SoundManager::getInstance();
    switch (row)
    {
    case 0:
    case 4:
        sm.playSFX("ui_click");
        break;
    case 1:
        sm.playSFX("sword_swing");
        break;
    case 2:
        sm.playSFX("zombie_hurt");
        break;
    case 3:
        break; // BGM 拖动时不插入音效，免得和正在放的音乐打架
    default:
        break;
    }
}
