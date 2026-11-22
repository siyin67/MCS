#include "SettingsState.h"
#include "../Statemanager.h"
#include "LanguageState.h"
#include "Localization/Localization.h"
#include "Localization/UiText.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "SoundOptionsState.h"
#include <iostream>
#include <memory>
#include <string>

namespace
{
// 与原版「选项」页一致：两列按钮，底部一条同宽的完成按钮
constexpr float BTN_W = 400.0f;
constexpr float BTN_H = 40.0f;
constexpr float GAP_X = 40.0f;
constexpr float FULL_W = BTN_W * 2.0f + GAP_X; // 840
constexpr float TOP_Y = 170.0f;
constexpr float DONE_H = 50.0f;
constexpr float DONE_MARGIN_BOTTOM = 60.0f;

void centerInRect(sf::Text &text, const sf::FloatRect &rect)
{
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setPosition({rect.position.x + (rect.size.x - bounds.size.x) / 2.0f - bounds.position.x,
                      rect.position.y + (rect.size.y - bounds.size.y) / 2.0f - bounds.position.y});
}
} // namespace

const char *SettingsState::itemKey(int index)
{
    return index == 0 ? "options.sound" : "options.language";
}

SettingsState::SettingsState()
    : font_("assets/fonts/minecraft.ttf"), titleText_(font_), doneText_(font_)
{
    std::cout << "SettingsState created" << std::endl;

    dim_.setFillColor(sf::Color(8, 8, 8, 238));

    titleText_.setCharacterSize(26);
    titleText_.setFillColor(sf::Color::White);
    titleText_.setOutlineColor(sf::Color(25, 25, 25));
    titleText_.setOutlineThickness(1.0f);

    ResourceManager &rm = ResourceManager::GetInstance();
    const bool hasButton = rm.HasTexture("menu_button");

    buttons_.resize(ITEM_COUNT);
    rects_.resize(ITEM_COUNT);
    texts_.reserve(ITEM_COUNT);

    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        if (hasButton)
            buttons_[i].setTexture(&rm.GetTexture("menu_button"));
        else
            buttons_[i].setFillColor(sf::Color(130, 130, 130));
        buttons_[i].setFillColor(sf::Color::White);
        buttons_[i].setSize({BTN_W, BTN_H});
        buttons_[i].setOutlineColor(sf::Color::White);

        texts_.emplace_back(font_);
        texts_.back().setCharacterSize(20);
        texts_.back().setFillColor(sf::Color::White);
        texts_.back().setOutlineColor(sf::Color(25, 25, 25));
        texts_.back().setOutlineThickness(1.0f);
    }

    if (hasButton)
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

void SettingsState::applyLanguage()
{
    langVersion_ = Localization::getInstance().version();
    ui_text::set(titleText_, tr("options.title"));
    ui_text::set(doneText_, tr("options.done"));
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        ui_text::set(texts_[i], tr(itemKey(i)));
    }
}

void SettingsState::updateLayout(sf::Vector2u winSize)
{
    const float cx = winSize.x / 2.0f;
    const float left = cx - FULL_W / 2.0f;

    dim_.setSize({static_cast<float>(winSize.x), static_cast<float>(winSize.y)});
    dim_.setPosition({0.0f, 0.0f});
    titleText_.setPosition({cx - titleText_.getLocalBounds().size.x / 2.0f, 54.0f});

    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        const float x = left + static_cast<float>(i) * (BTN_W + GAP_X);
        const float y = TOP_Y;
        rects_[i] = sf::FloatRect({x, y}, {BTN_W, BTN_H});
        buttons_[i].setPosition({x, y});
        buttons_[i].setOutlineThickness(hoverIndex_ == i ? 2.0f : 0.0f);
        centerInRect(texts_[i], rects_[i]);
    }

    doneButton_.setPosition({left, static_cast<float>(winSize.y) - DONE_H - DONE_MARGIN_BOTTOM});
    doneButton_.setOutlineThickness(doneHover_ ? 2.0f : 0.0f);
    centerInRect(doneText_, doneButton_.getGlobalBounds());
}

void SettingsState::handleInput(sf::RenderWindow &window)
{
    const sf::Vector2u winSize = window.getSize();
    window.setView(sf::View(sf::Vector2f(winSize.x / 2.f, winSize.y / 2.f), sf::Vector2f(winSize)));

    const sf::Vector2f mouseCoords = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    const bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    hoverIndex_ = -1;
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        if (rects_[i].contains(mouseCoords))
        {
            hoverIndex_ = i;
        }
    }
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
        if (hoverIndex_ >= 0)
        {
            openItem(hoverIndex_);
            return;
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

void SettingsState::openItem(int index)
{
    SoundManager::getInstance().playSFX("ui_click");
    if (index == 0)
    {
        stateManager_->pushState(std::make_unique<SoundOptionsState>());
    }
    else
    {
        stateManager_->pushState(std::make_unique<LanguageState>());
    }
}

void SettingsState::update(float deltaTime)
{
    (void)deltaTime;
}

void SettingsState::render(sf::RenderWindow &window)
{
    if (langVersion_ != Localization::getInstance().version())
    {
        applyLanguage();
        updateLayout(window.getSize());
    }

    window.draw(dim_);
    window.draw(titleText_);
    for (int i = 0; i < ITEM_COUNT; ++i)
    {
        window.draw(buttons_[i]);
        window.draw(texts_[i]);
    }
    window.draw(doneButton_);
    window.draw(doneText_);
}
