#pragma once
#include <SFML/Graphics.hpp>

class StateManager;

class IState
{
protected:
    StateManager *stateManager_ = nullptr;

public:
    virtual ~IState() = default;

    void setStateManager(StateManager *sm)
    {
        stateManager_ = sm;
    }

    virtual void handleInput(sf::RenderWindow &window) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow &window) = 0;
};