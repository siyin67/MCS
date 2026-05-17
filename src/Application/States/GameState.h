#pragma once
#include "IState.h"
#include <SFML/Graphics.hpp>
#include "Player/player.h"
#include "Monster/Monster.h"
#include "Monster/Spawner.h"

class GameState : public IState
{
private:
    sf::RectangleShape background_;
    sf::View camera_;
    Player player_;
    std::vector<std::unique_ptr<Monster>> monsters_;
    Spawner spawner_;

public:
    GameState();
    void handleInput(sf::RenderWindow &window) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window) override;
};