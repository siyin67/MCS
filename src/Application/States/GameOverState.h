#pragma once
#include "IState.h"
#include <SFML/Graphics.hpp>

class GameOverState : public IState
{
private:
    sf::RectangleShape background_;
    sf::RectangleShape button_;
    sf::Font font_;
    sf::Text gameOverText_;
    sf::Text buttonText_;

public:
    GameOverState();
    void handleInput(sf::RenderWindow &window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window) override;
};