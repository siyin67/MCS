#pragma once
#include <memory>
#include <vector>  // 改用 vector
#include "States/IState.h"

class StateManager
{
private:
    std::vector<std::unique_ptr<IState>> states_;  // vector 代替 stack

public:
    StateManager() = default;
    ~StateManager() = default;

    StateManager(const StateManager&) = delete;
    StateManager& operator=(const StateManager&) = delete;

    void pushState(std::unique_ptr<IState> state);
    void popState();
    void changeState(std::unique_ptr<IState> state);

    IState* getCurrentState();
    bool isEmpty() const;

    void handleInput(sf::RenderWindow& window);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
};