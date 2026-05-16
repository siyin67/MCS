#include "PauseState.h"
#include "../Statemanager.h"
#include <iostream>

PauseState::PauseState()
{
    std::cout << "PauseState created" << std::endl;

    // 半透明黑色背景
    background_.setFillColor(sf::Color(0, 0, 0, 180));
    background_.setSize({1280, 720});
}

void PauseState::handleInput(sf::RenderWindow &window)
{
    static bool wasPressed = false;
    bool isPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) ||
                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

    if (isPressed && !wasPressed)
    {
        std::cout << "Resume game" << std::endl;
        stateManager_->popState();
    }

    wasPressed = isPressed;
}

void PauseState::update(float deltaTime)
{
    (void)deltaTime;
}

void PauseState::render(sf::RenderWindow &window)
{
    // 获取当前视图的大小和位置
    sf::View view = window.getView();
    sf::Vector2f viewSize = view.getSize();
    sf::Vector2f viewCenter = view.getCenter();

    // 设置背景覆盖整个视图
    background_.setSize(viewSize);
    background_.setPosition({viewCenter.x - viewSize.x / 2,
                             viewCenter.y - viewSize.y / 2});

    window.draw(background_);
}