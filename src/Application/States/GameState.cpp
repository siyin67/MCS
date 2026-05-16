// GameState.cpp
#include "GameState.h"
#include "MenuState.h"
#include "PauseState.h"
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
    /*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
    {
        stateManager_->changeState(std::make_unique<MenuState>());
        // stateManager_->changeState(std::move(std::make_unique<MenuState>()));
        return;
    }*/
    // 按 ESC 暂停（不是直接返回菜单）
    static bool wasPressed = false;
    bool isPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);

    if (isPressed && !wasPressed)
    {
        std::cout << "Game paused" << std::endl;
        stateManager_->pushState(std::make_unique<PauseState>());
    }

    wasPressed = isPressed;
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