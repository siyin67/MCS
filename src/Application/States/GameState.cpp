// GameState.cpp
#include "GameState.h"
#include "Resourcemanager/ResourceManager.h"
#include <iostream>
#include <cmath>

GameState::GameState()
{
    std::cout << "GameState constructor called" << std::endl;
    background_.setFillColor(sf::Color(80, 80, 80));
    background_.setSize({1280, 720});
    background_.setPosition({0, 0});
    camera_.setCenter(player_.getPosition());
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
    window.setView(camera_);
    player_.render(window);
}