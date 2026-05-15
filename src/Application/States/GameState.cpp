// GameState.cpp
#include "GameState.h"
#include "Resourcemanager/ResourceManager.h"
#include <iostream>
#include <cmath>

GameState::GameState()
{
    std::cout << "GameState constructor START" << std::endl;
    background_.setFillColor(sf::Color::Blue);
    background_.setSize({5000, 5000});
    background_.setPosition({0, 0});
    camera_.setCenter(player_.getPosition());
    std::cout << "GameState constructor END" << std::endl;
}

void GameState::handleInput(sf::RenderWindow &window)
{
}

void GameState::update(float deltaTime)
{
    player_.update(deltaTime);
    camera_.setCenter(player_.getPosition());
}

void GameState::render(sf::RenderWindow &window)
{
    window.draw(background_);
    player_.render(window);
    window.setView(camera_);
}