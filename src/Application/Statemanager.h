#pragma once
#include <memory>
#include <vector>
#include "States/IState.h"

class StateManager
{
private:
    std::vector<std::unique_ptr<IState>> states_;

public:
    StateManager() = default;
    ~StateManager() = default;

    StateManager(const StateManager &) = delete;
    StateManager &operator=(const StateManager &) = delete;

    void pushState(std::unique_ptr<IState> state);
    void popState();
    void changeState(std::unique_ptr<IState> state);
    void clearAndPush(std::unique_ptr<IState> state); // 新增：清空栈并压入

    IState *getCurrentState();
    bool isEmpty() const;

    void handleInput(sf::RenderWindow &window);
    void update(float deltaTime);
    void render(sf::RenderWindow &window);
};