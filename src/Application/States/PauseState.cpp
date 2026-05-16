// PauseState.cpp
#include "PauseState.h"
#include "../Statemanager.h"
#include <iostream>
#include "MenuState.h"
#include "Resourcemanager/ResourceManager.h"

PauseState::PauseState() : font_("assets/fonts/minecraft.ttf"),
                           resumeText_(font_),
                           exit_Text_(font_),
                           pauseText_(font_)
{
    std::cout << "PauseState created" << std::endl;

    if (ResourceManager::GetInstance().HasTexture("menu_button"))
    {
        sf::Texture &btnTex = ResourceManager::GetInstance().GetTexture("menu_button");
        button_resume_.setTexture(&btnTex);
        button_exit_.setTexture(&btnTex);
    }
    button_resume_.setSize({400, 50});
    button_exit_.setSize({400, 50});

    resumeText_.setString("Resume Game"); // 修正拼写
    resumeText_.setCharacterSize(28);
    resumeText_.setFillColor(sf::Color::White);

    exit_Text_.setString("Main Menu");
    exit_Text_.setCharacterSize(28);
    exit_Text_.setFillColor(sf::Color::White);

    pauseText_.setString("Pause");
    pauseText_.setCharacterSize(64);
    pauseText_.setFillColor(sf::Color::White);

    background_.setFillColor(sf::Color(0, 0, 0, 180));
    background_.setSize({1280, 720});

    // 初始化成员变量
    lastEscapePressed_ = false;
}

void PauseState::handleInput(sf::RenderWindow &window)
{
    /*
    static bool wasPressed = false;
    bool isPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) ||
                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

    if (isPressed && !wasPressed)
    {
        std::cout << "Resume game" << std::endl;
        stateManager_->popState();
    }

    wasPressed = isPressed;
    */

    static bool wasPressed = false;

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    float mouseX = static_cast<float>(mousePos.x);
    float mouseY = static_cast<float>(mousePos.y);

    // Resume 按钮（完全模仿 GameOverState）
    if (button_resume_.getGlobalBounds().contains({mouseX, mouseY}))
    {
        button_resume_.setOutlineThickness(3);
        button_resume_.setOutlineColor(sf::Color::White);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !wasPressed)
        {
            std::cout << "Resume button clicked" << std::endl;
            stateManager_->popState();
            return;
        }
    }
    else
    {
        button_resume_.setOutlineThickness(0);
    }

    // Exit 按钮（完全模仿 GameOverState）
    if (button_exit_.getGlobalBounds().contains({mouseX, mouseY}))
    {
        button_exit_.setOutlineThickness(3);
        button_exit_.setOutlineColor(sf::Color::White);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !wasPressed)
        {
            sf::Mouse::setPosition(sf::Vector2i(0, 0), window);
            std::cout << "Exit button clicked" << std::endl;
            stateManager_->clearAndPush(std::make_unique<MenuState>());
            return;
        }
    }
    else
    {
        button_exit_.setOutlineThickness(0);
    }
    wasPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
}

void PauseState::update(float deltaTime)
{
    (void)deltaTime;
}

void PauseState::render(sf::RenderWindow &window)
{
    window.setView(window.getDefaultView());

    sf::Vector2u windowSize = window.getSize();
    float centerX = windowSize.x / 2.0f;
    float centerY = windowSize.y / 2.0f;

    background_.setSize(sf::Vector2f(windowSize.x, windowSize.y));
    background_.setPosition({0, 0});
    window.draw(background_);

    sf::FloatRect pauseTextBounds = pauseText_.getLocalBounds();
    pauseText_.setPosition({centerX - pauseTextBounds.size.x / 2, centerY - 200});
    window.draw(pauseText_);

    float buttonWidth = 400;
    float buttonHeight = 50;
    float verticalSpacing = 20;

    button_resume_.setSize({buttonWidth, buttonHeight});
    button_resume_.setPosition({centerX - buttonWidth / 2, centerY + 40});
    window.draw(button_resume_);

    sf::FloatRect resumeTextBounds = resumeText_.getLocalBounds();
    resumeText_.setPosition({centerX - resumeTextBounds.size.x / 2,
                             centerY + 40 + buttonHeight / 2 - resumeTextBounds.size.y / 2});
    window.draw(resumeText_);

    button_exit_.setSize({buttonWidth, buttonHeight});
    button_exit_.setPosition({centerX - buttonWidth / 2, centerY + 40 + buttonHeight + verticalSpacing});
    window.draw(button_exit_);

    sf::FloatRect exitTextBounds = exit_Text_.getLocalBounds();
    exit_Text_.setPosition({centerX - exitTextBounds.size.x / 2,
                            centerY + 40 + buttonHeight + verticalSpacing + buttonHeight / 2 - exitTextBounds.size.y / 2});
    window.draw(exit_Text_);
}