// PauseState.h
#pragma once
#include "IState.h"
#include <SFML/Graphics.hpp>

class PauseState : public IState
{
private:
    sf::RectangleShape background_;
    sf::RectangleShape button_resume_;
    sf::RectangleShape button_settings_;
    sf::RectangleShape button_exit_;
    sf::Font font_;
    sf::Text resumeText_;
    sf::Text settingsText_;
    sf::Text exit_Text_;
    sf::Text pauseText_;

    // 添加成员变量用于防抖
    bool lastEscapePressed_ = false;

public:
    PauseState();
    void handleInput(sf::RenderWindow &window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window) override;

private:
    void updateLayout(sf::Vector2u winSize);
    void applyLanguage();
    int langVersion_ = -1;
};