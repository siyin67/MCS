#pragma once
#include "IState.h"
#include <SFML/Graphics.hpp>
#include "Player/player.h"

class GameState : public IState
{
private:
    sf::RectangleShape background_;
    sf::View camera_;
    Player player_;

public:
    GameState();
    void handleInput(sf::RenderWindow &window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window) override;
};