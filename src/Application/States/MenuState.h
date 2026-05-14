#pragma once
#include "IState.h"
#include <SFML/Graphics.hpp>

class MenuState : public IState
{
private:
    sf::Texture backgroundImage_;
    sf::RectangleShape background_;
    sf::RectangleShape button_;

public:
    MenuState();
    ~MenuState() override = default;

    void handleInput() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window) override;
};