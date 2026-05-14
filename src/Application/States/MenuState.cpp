#include "MenuState.h"
#include "../StateManager.h"
#include <iostream>

MenuState::MenuState()
{
    if (!backgroundImage_.loadFromFile("assets/images/backgrounds/menubackground.png"))
    {
        // 加载失败时的处理（可选）
        std::cout << "Warning: Failed to load menu background" << std::endl;
    }
    else
    {
        std::cout << "Menu background loaded successfully" << std::endl;
    }
    background_.setTexture(&backgroundImage_);
    background_.setSize({1280, 720});
    background_.setPosition({0, 0});
    std::cout << "MenuState created" << std::endl;
}

void MenuState::handleInput()
{
}

void MenuState::update(float deltaTime)
{
    (void)deltaTime;
}

void MenuState::render(sf::RenderWindow &window)
{
    sf::Vector2u windowSize = window.getSize();
    background_.setSize(sf::Vector2f(windowSize.x, windowSize.y));
    window.draw(background_);
    // window.draw(button_);
}