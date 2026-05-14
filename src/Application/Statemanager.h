#pragma once
#include <memory>
#include <stack>
#include "States/IState.h"

class StateManager
{
private:
    std::stack<std::unique_ptr<IState>> states_; // 状态栈

public:
    StateManager() = default;
    ~StateManager() = default;

    StateManager(const StateManager &) = delete;
    StateManager &operator=(const StateManager &) = delete;

    void pushState(std::unique_ptr<IState> state);

    void popState();

    void changeState(std::unique_ptr<IState> state);

    IState *getCurrentState();

    bool isEmpty() const;

    void handleInput();
    void update(float deltaTime);
    void render(sf::RenderWindow &window);
};