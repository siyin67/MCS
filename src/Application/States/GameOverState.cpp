#include "GameOverState.h"
#include "../StateManager.h"
#include "MenuState.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include <iostream>

GameOverState::GameOverState()
    : font_("assets/fonts/minecraft.ttf"),
      gameOverText_(font_), buttonText_(font_)
{
    std::cout << "GameOverState created" << std::endl;

    // 半透明深红色背景
    background_.setFillColor(sf::Color(100, 0, 0, 180));

    // 使用 menu_button 纹理（与 MenuState 共用）
    if (ResourceManager::GetInstance().HasTexture("menu_button"))
    {
        sf::Texture &btnTex = ResourceManager::GetInstance().GetTexture("menu_button");
        button_.setTexture(&btnTex);
    }
    button_.setSize({400, 50});

    // "GAME OVER" 标题
    gameOverText_.setString("GAME OVER");
    gameOverText_.setCharacterSize(64);
    gameOverText_.setFillColor(sf::Color::White);

    // 按钮文字
    buttonText_.setString("Main Menu");
    buttonText_.setCharacterSize(28);
    buttonText_.setFillColor(sf::Color::White);
    SoundManager::getInstance().stopBGM();
    SoundManager::getInstance().playSFX("gameover");
}

void GameOverState::handleInput(sf::RenderWindow &window)
{
    static bool wasPressed = false;

    // ========== 鼠标点击按钮 ==========
    // 获取鼠标位置
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    float mouseX = static_cast<float>(mousePos.x);
    float mouseY = static_cast<float>(mousePos.y);

    // 检查鼠标是否在按钮上
    if (button_.getGlobalBounds().contains({mouseX, mouseY}))
    {
        // 按钮高亮
        button_.setOutlineThickness(3);
        button_.setOutlineColor(sf::Color::White);

        // 鼠标点击
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !wasPressed)
        {
            sf::Mouse::setPosition(sf::Vector2i(0, 0), window);
            std::cout << "Button clicked, back to menu" << std::endl;
            stateManager_->clearAndPush(std::make_unique<MenuState>());
            return;
        }
    }
    else
    {
        button_.setOutlineThickness(0);
    }
}

void GameOverState::update(float deltaTime)
{
    (void)deltaTime;
}

void GameOverState::render(sf::RenderWindow &window)
{
    window.setView(window.getDefaultView());

    sf::Vector2u windowSize = window.getSize();
    float centerX = windowSize.x / 2.0f;
    float centerY = windowSize.y / 2.0f;

    // 背景覆盖全屏
    background_.setSize(sf::Vector2f(windowSize.x, windowSize.y));
    background_.setPosition({0, 0});
    window.draw(background_);

    // 绘制 "GAME OVER"（居中）
    sf::FloatRect textBounds = gameOverText_.getLocalBounds();
    gameOverText_.setPosition({centerX - textBounds.size.x / 2, centerY - 200});
    window.draw(gameOverText_);

    // 绘制按钮
    float buttonWidth = 400;
    float buttonHeight = 50;
    button_.setSize({buttonWidth, buttonHeight});
    button_.setPosition({centerX - buttonWidth / 2, centerY + 40});
    window.draw(button_);

    // 绘制按钮文字（居中于按钮）
    sf::FloatRect btnTextBounds = buttonText_.getLocalBounds();
    buttonText_.setPosition({centerX - btnTextBounds.size.x / 2, centerY + 40 + buttonHeight / 2 - btnTextBounds.size.y / 2});
    window.draw(buttonText_);
}