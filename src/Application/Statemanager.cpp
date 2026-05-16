#include "StateManager.h"

void StateManager::pushState(std::unique_ptr<IState> state)
{
    if (state)
    {
        state->setStateManager(this);
        states_.push_back(std::move(state));
    }
}

void StateManager::popState()
{
    if (!states_.empty())
    {
        states_.pop_back();
    }
}

void StateManager::changeState(std::unique_ptr<IState> state)
{
    if (!states_.empty())
    {
        states_.pop_back();
    }
    pushState(std::move(state));
}

void StateManager::clearAndPush(std::unique_ptr<IState> state)
{
    states_.clear(); // 清空整个栈
    if (state)
    {
        state->setStateManager(this);
        states_.push_back(std::move(state));
    }
}

IState *StateManager::getCurrentState()
{
    if (states_.empty())
        return nullptr;
    return states_.back().get();
}

bool StateManager::isEmpty() const
{
    return states_.empty();
}

void StateManager::handleInput(sf::RenderWindow &window)
{
    if (getCurrentState())
        getCurrentState()->handleInput(window);
}

void StateManager::update(float deltaTime)
{
    if (getCurrentState())
        getCurrentState()->update(deltaTime);
}

void StateManager::render(sf::RenderWindow &window)
{
    for (auto &state : states_)
    {
        state->render(window);
    }
}