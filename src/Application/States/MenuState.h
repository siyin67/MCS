#pragma once
#include "IState.h"
#include <SFML/Graphics.hpp>

class MenuState : public IState
{
private:
    sf::Texture backgroundImage_;
    sf::RectangleShape background_;
    sf::RectangleShape title_;
    sf::RectangleShape start_button_;
    sf::RectangleShape exit_button_;
    sf::Font font_;
    sf::Text text_start_;
    sf::Text text_exit_;

public:
    MenuState();
    ~MenuState() override = default;

    void handleInput(sf::RenderWindow &window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window) override;
};