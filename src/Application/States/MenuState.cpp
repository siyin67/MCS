#include "MenuState.h"
#include "../StateManager.h"
#include <iostream>
#include "Resourcemanager/ResourceManager.h"
#include "GameState.h"

MenuState::MenuState() : font_("assets/fonts/minecraft.ttf"), text_start_(font_), text_exit_(font_)

{
    if (ResourceManager::GetInstance().HasTexture("menu_button"))
    {
        sf::Texture &bgTex = ResourceManager::GetInstance().GetTexture("menu_button");
        start_button_.setTexture(&bgTex);
        exit_button_.setTexture(&bgTex);
    }
    if (ResourceManager::GetInstance().HasTexture("menu_title"))
    {
        sf::Texture &bgTex = ResourceManager::GetInstance().GetTexture("menu_title");
        title_.setTexture(&bgTex);
    }
    if (ResourceManager::GetInstance().HasTexture("menu_bg"))
    {
        sf::Texture &bgTex = ResourceManager::GetInstance().GetTexture("menu_bg");
        background_.setTexture(&bgTex);
    }
    else
    {
        std::cout << "Warning: menu_bg texture not found in resource manager" << std::endl;
        // 可选：设置一个默认颜色
        background_.setFillColor(sf::Color(50, 50, 100));
    }
    text_start_.setString("Start Game");
    text_exit_.setString("Exit Game");
    start_button_.setSize({400, 50});
    exit_button_.setSize({400, 50});
    title_.setSize({700, 250});

    text_start_.setCharacterSize(20);
    text_exit_.setCharacterSize(20);
    text_start_.setFillColor(sf::Color::White);
    text_exit_.setFillColor(sf::Color::White);
    background_.setSize({1280, 720});
    background_.setPosition({0, 0});
}

void MenuState::handleInput(sf::RenderWindow &window)
{
    // 获取鼠标位置
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    // 检查鼠标是否在开始按钮上
    if (start_button_.getGlobalBounds().contains({static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)}))
    {
        start_button_.setOutlineThickness(3);
        start_button_.setOutlineColor(sf::Color::White);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {

            auto gameState = std::make_unique<GameState>();

            stateManager_->changeState(std::move(gameState));

            return;
        }
    }
    else
    {
        start_button_.setOutlineThickness(0);
    }

    // 检查鼠标是否在退出按钮上
    if (exit_button_.getGlobalBounds().contains({static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)}))
    {
        exit_button_.setOutlineThickness(3);
        exit_button_.setOutlineColor(sf::Color::White);
        // 点击退出游戏
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            std::cout << "Exit Game" << std::endl;
            exit(0); // 退出程序
        }
    }
    else
    {
        // 恢复默认边框（无边框或细边框）
        exit_button_.setOutlineThickness(0);
    }
}
void MenuState::update(float deltaTime)
{
    (void)deltaTime;
}

void MenuState::render(sf::RenderWindow &window)
{
    sf::Vector2u windowSize = window.getSize();
    background_.setSize(sf::Vector2f(windowSize.x, windowSize.y));
    // 设置文字和按钮位置文字分别是开始游戏和退出游戏位于两个按钮正中央两个按钮中间适当间隔排列在整个窗口中部偏离中线一点
    text_start_.setPosition({windowSize.x / 2 - text_start_.getGlobalBounds().size.x / 2, windowSize.y / 2 - text_start_.getGlobalBounds().size.y / 2 + 50});
    text_exit_.setPosition({windowSize.x / 2 - text_exit_.getGlobalBounds().size.x / 2, windowSize.y / 2 - text_exit_.getGlobalBounds().size.y / 2 + 120});
    start_button_.setPosition({windowSize.x / 2 - start_button_.getGlobalBounds().size.x / 2, windowSize.y / 2 - start_button_.getGlobalBounds().size.y / 2 + 50});
    exit_button_.setPosition({windowSize.x / 2 - exit_button_.getGlobalBounds().size.x / 2, windowSize.y / 2 - exit_button_.getGlobalBounds().size.y / 2 + 120});
    title_.setPosition({windowSize.x / 2 - title_.getGlobalBounds().size.x / 2, windowSize.y / 2 - title_.getGlobalBounds().size.y / 2 - 150});
    window.draw(background_);
    window.draw(start_button_);
    window.draw(exit_button_);
    window.draw(title_);
    window.draw(text_start_);
    window.draw(text_exit_);

    // window.draw(button_);
}