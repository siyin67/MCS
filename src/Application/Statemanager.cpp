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
    // 从底向上全部渲染（叠加效果）
    for (auto &state : states_)
    {
        state->render(window);
    }
}