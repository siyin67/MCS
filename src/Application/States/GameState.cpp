// GameState.cpp
#include "GameState.h"
#include "MenuState.h"
#include "PauseState.h"
#include "GameOverState.h"
#include "Resourcemanager/ResourceManager.h"
#include "../Statemanager.h"
#include <iostream>
#include <cmath>

GameState::GameState()
{
    std::cout << "GameState constructor START" << std::endl;
    if (ResourceManager::GetInstance().HasTexture("map1"))
    {
        sf::Texture &bgTex = ResourceManager::GetInstance().GetTexture("map1");
        background_.setTexture(&bgTex);
    }
    // background_.setFillColor(sf::Color::Blue);
    background_.setSize({5000, 5000});
    background_.setPosition({0, 0});
    camera_.setCenter(player_.getPosition());
    camera_.setSize({1920, 1080});
    std::cout << "GameState constructor END" << std::endl;
}

void GameState::handleInput(sf::RenderWindow &window)
{
    static bool wasPressed = false;

    // 按 ESC 暂停
    bool escPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
    if (escPressed && !wasPressed)
    {
        std::cout << "Game paused" << std::endl;
        stateManager_->pushState(std::make_unique<PauseState>());
    }

    // 按数字 0 触发游戏结束
    bool zeroPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num0);
    if (zeroPressed && !wasPressed)
    {
        std::cout << "Game over triggered by 0 key" << std::endl;
        stateManager_->pushState(std::make_unique<GameOverState>());
    }

    wasPressed = escPressed || zeroPressed;
}

void GameState::update(float deltaTime)
{
    player_.update(deltaTime);
    camera_.setCenter(player_.getPosition());
}

void GameState::render(sf::RenderWindow &window)
{
    window.setView(camera_);
    window.draw(background_);
    player_.render(window);
}