#pragma once
#include "IState.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <atomic>
#include <thread>

class MenuState : public IState
{
private:
    sf::Texture backgroundImage_;
    sf::RectangleShape background_;
    sf::RectangleShape title_;
    sf::RectangleShape start_button_;
    sf::RectangleShape settings_button_;
    sf::RectangleShape exit_button_;
    sf::RectangleShape input_bg_;
    sf::RectangleShape confirm_button_;
    sf::RectangleShape head_preview_;
    sf::Font font_;
    sf::Text text_start_;
    sf::Text text_settings_;
    sf::Text text_exit_;
    sf::Text input_text_;
    std::string playerName_;
    bool headLoaded_;
    std::atomic<bool> headLoading_{false};
    std::atomic<bool> headReady_{false};
    std::thread headThread_;
    std::string headResultPath_;
    bool headResultOk_ = false;

public:
    MenuState();
    ~MenuState() override;

    void handleInput(sf::RenderWindow &window) override;
    void handleEvent(const sf::Event &event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window) override;

private:
    void updateLayout(sf::Vector2u winSize);
    void tryLoadHead();
    void applyLanguage();
    int langVersion_ = -1;
};
