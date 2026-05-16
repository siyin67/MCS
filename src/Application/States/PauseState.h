#pragma once
#include "IState.h"
#include <SFML/Graphics.hpp>

class PauseState : public IState
{
private:
    sf::RectangleShape background_;

public:
    PauseState();
    void handleInput(sf::RenderWindow &window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window) override;
};