#include "LanguageState.h"
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
constexpr float SCROLLBAR_W = 6.0f;
constexpr float SCROLLBAR_GAP = 12.0f;
constexpr float SCROLL_MIN_THUMB = 24.0f;

void centerInRect(sf::Text &text, const sf::FloatRect &rect)
{
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setPosition({rect.position.x + (rect.size.x - bounds.size.x) / 2.0f - bounds.position.x,
                      rect.position.y + (rect.size.y - bounds.size.y) / 2.0f - bounds.position.y});
}
} // namespace

LanguageState::LanguageState()
    : font_("assets/fonts/minecraft.ttf"), titleText_(font_), noteText_(font_), doneText_(font_)
{
    std::cout << "LanguageState created" << std::endl;

    dim_.setFillColor(sf::Color(12, 12, 14, 255)); // 二级页用不透明底，避免上一级页面的按钮透出来

    titleText_.setCharacterSize(26);
    titleText_.setFillColor(sf::Color::White);
    titleText_.setOutlineColor(sf::Color(25, 25, 25));
    titleText_.setOutlineThickness(1.0f);

    noteText_.setCharacterSize(18);
    noteText_.setFillColor(sf::Color(200, 200, 200));
    noteText_.setOutlineColor(sf::Color(25, 25, 25));
    noteText_.setOutlineThickness(1.0f);

    ResourceManager &rm = ResourceManager::GetInstance();
    if (rm.HasTexture("menu_button"))
        doneButton_.setTexture(&rm.GetTexture("menu_button"));
    else
        doneButton_.setFillColor(sf::Color(130, 130, 130));
    doneButton_.setFillColor(sf::Color::White);
    doneButton_.setSize({DONE_W, DONE_H});
    doneButton_.setOutlineColor(sf::Color::White);

    doneText_.setCharacterSize(24);
    doneText_.setFillColor(sf::Color::White);
    doneText_.setOutlineColor(sf::Color(25, 25, 25));
    doneText_.setOutlineThickness(1.0f);

    scrollTrack_.setFillColor(sf::Color(0, 0, 0, 120));
    scrollThumb_.setFillColor(sf::Color(190, 190, 190));

    rebuildRows();
    applyLanguage();
}

void LanguageState::rebuildRows()
{
    const Localization &loc = Localization::getInstance();
    const std::vector<Localization::LanguageInfo> &langs = loc.languages();

    rowBoxes_.clear();
    rowTexts_.clear();
    rowRects_.clear();
    codes_.clear();

    rowBoxes_.resize(langs.size());
    rowRects_.resize(langs.size());
    rowTexts_.reserve(langs.size());

    for (std::size_t i = 0; i < langs.size(); ++i)
    {
        codes_.push_back(langs[i].code);

        rowBoxes_[i].setFillColor(sf::Color(0, 0, 0, 0));
        rowRects_[i] = sf::FloatRect({0.0f, 0.0f}, {LIST_W, ROW_H});

        rowTexts_.emplace_back(font_);
        rowTexts_.back().setCharacterSize(20);
        rowTexts_.back().setFillColor(sf::Color::White);
        rowTexts_.back().setOutlineColor(sf::Color(25, 25, 25));
        rowTexts_.back().setOutlineThickness(1.0f);
        ui_text::set(rowTexts_.back(), langs[i].display);
    }

    clampScroll();
}

void LanguageState::applyLanguage()
{
    langVersion_ = Localization::getInstance().version();
    ui_text::set(titleText_, tr("language.title"));
    ui_text::set(noteText_, tr("language.note"));
    ui_text::set(doneText_, tr("language.done"));
}

void LanguageState::updateLayout(sf::Vector2u winSize)
{
    const float cx = winSize.x / 2.0f;
    const float listLeft = cx - LIST_W / 2.0f;

    dim_.setSize({static_cast<float>(winSize.x), static_cast<float>(winSize.y)});
    dim_.setPosition({0.0f, 0.0f});

    titleText_.setPosition({cx - titleText_.getLocalBounds().size.x / 2.0f, 54.0f});

    viewTop_ = LIST_TOP;
    viewBottom_ = static_cast<float>(winSize.y) - LIST_BOTTOM_MARGIN;
    clampScroll();

    const float contentH = static_cast<float>(rowRects_.size()) * ROW_H;
    const float viewH = viewBottom_ - viewTop_;
    const float startY = viewTop_ + std::max(0.0f, (viewH - contentH) / 2.0f) - scrollOffset_;

    const std::string &current = Localization::getInstance().getLanguage();

    for (std::size_t i = 0; i < rowRects_.size(); ++i)
    {
        const float y = startY + static_cast<float>(i) * ROW_H;
        rowRects_[i] = sf::FloatRect({listLeft, y}, {LIST_W, ROW_H});
        rowBoxes_[i].setSize({LIST_W, ROW_H});
        rowBoxes_[i].setPosition({listLeft, y});

        const bool selected = (codes_[i] == current);
        if (selected)
        {
            rowBoxes_[i].setFillColor(sf::Color(70, 70, 76, 235)); // 原版：选中项套深灰框（深色页面上要能看出来）
        }
        else if (hoverIndex_ == static_cast<int>(i))
        {
            rowBoxes_[i].setFillColor(sf::Color(255, 255, 255, 40));
        }
        else
        {
            rowBoxes_[i].setFillColor(sf::Color(0, 0, 0, 0));
        }

        centerInRect(rowTexts_[i], rowRects_[i]);
        // 超出可视区的条目裁掉（简单起见只做半透明渐隐，不做裁剪矩阵）
        const bool visible = (y + ROW_H > viewTop_) && (y < viewBottom_);
        rowTexts_[i].setFillColor(visible ? sf::Color::White : sf::Color(255, 255, 255, 0));
        rowBoxes_[i].setOutlineColor(sf::Color::Transparent);
    }

    // 滚动条（条目超出可视区时才画）
    const float trackX = listLeft + LIST_W + SCROLLBAR_GAP;
    if (contentH > viewH)
    {
        scrollTrack_.setSize({SCROLLBAR_W, viewH});
        scrollTrack_.setPosition({trackX, viewTop_});

        const float ratio = viewH / contentH;
        const float thumbH = std::max(SCROLL_MIN_THUMB, viewH * ratio);
        const float maxOff = maxScroll();
        const float t = maxOff > 0.0f ? (scrollOffset_ / maxOff) : 0.0f;
        scrollThumb_.setSize({SCROLLBAR_W, thumbH});
        scrollThumb_.setPosition({trackX, viewTop_ + t * (viewH - thumbH)});
    }
    else
    {
        scrollTrack_.setSize({0.0f, 0.0f});
        scrollThumb_.setSize({0.0f, 0.0f});
    }

    const float doneY = static_cast<float>(winSize.y) - DONE_H - 60.0f;
    noteText_.setPosition({cx - noteText_.getLocalBounds().size.x / 2.0f, doneY - 40.0f});

    doneButton_.setPosition({cx - DONE_W / 2.0f, doneY});
    doneButton_.setOutlineThickness(doneHover_ ? 2.0f : 0.0f);
    centerInRect(doneText_, doneButton_.getGlobalBounds());
}

float LanguageState::maxScroll() const
{
    const float contentH = static_cast<float>(rowRects_.size()) * ROW_H;
    const float viewH = std::max(1.0f, viewBottom_ - viewTop_);
    return std::max(0.0f, contentH - viewH);
}

void LanguageState::clampScroll()
{
    scrollOffset_ = std::clamp(scrollOffset_, 0.0f, maxScroll());
}

void LanguageState::handleEvent(const sf::Event &event)
{
    if (const auto *wheel = event.getIf<sf::Event::MouseWheelScrolled>())
    {
        if (wheel->delta != 0.0f)
        {
            scrollOffset_ -= wheel->delta * ROW_H * 2.0f;
            clampScroll();
        }
    }
}

int LanguageState::hitTestRow(sf::Vector2f mouse) const
{
    if (mouse.y < viewTop_ || mouse.y > viewBottom_)
    {
        return -1;
    }
    for (std::size_t i = 0; i < rowRects_.size(); ++i)
    {
        if (rowRects_[i].contains(mouse))
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void LanguageState::handleInput(sf::RenderWindow &window)
{
    const sf::Vector2u winSize = window.getSize();
    window.setView(sf::View(sf::Vector2f(winSize.x / 2.f, winSize.y / 2.f), sf::Vector2f(winSize)));

    const sf::Vector2f mouseCoords = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    const bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    hoverIndex_ = hitTestRow(mouseCoords);
    doneHover_ = doneButton_.getGlobalBounds().contains(mouseCoords);

    const bool mousePressed = mouseDown && !mouseWasDown_;
    mouseWasDown_ = mouseDown;

    if (mousePressed)
    {
        if (doneButton_.getGlobalBounds().contains(mouseCoords))
        {
            SoundManager::getInstance().playSFX("ui_click");
            stateManager_->popState();
            return;
        }
        if (hoverIndex_ >= 0 && hoverIndex_ < static_cast<int>(codes_.size()))
        {
            SoundManager::getInstance().playSFX("ui_click");
            Localization::getInstance().setLanguage(codes_[hoverIndex_]);
            applyLanguage();
            rebuildRows();
        }
    }

    const bool escPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
    if (escPressed && !escWasDown_)
    {
        SoundManager::getInstance().playSFX("ui_click");
        stateManager_->popState();
        return;
    }
    escWasDown_ = escPressed;

    updateLayout(winSize);
}

void LanguageState::update(float deltaTime)
{
    (void)deltaTime;
}

void LanguageState::render(sf::RenderWindow &window)
{
    if (langVersion_ != Localization::getInstance().version())
    {
        applyLanguage();
        updateLayout(window.getSize());
    }

    window.draw(dim_);
    window.draw(titleText_);

    for (std::size_t i = 0; i < rowRects_.size(); ++i)
    {
        const float y = rowRects_[i].position.y;
        if (y + ROW_H <= viewTop_ || y >= viewBottom_)
        {
            continue; // 滚出可视区的条目不画
        }
        window.draw(rowBoxes_[i]);
        window.draw(rowTexts_[i]);
    }

    window.draw(scrollTrack_);
    window.draw(scrollThumb_);
    window.draw(noteText_);
    window.draw(doneButton_);
    window.draw(doneText_);
}
