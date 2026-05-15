#include "StateManager.h"

void StateManager::pushState(std::unique_ptr<IState> state)
{
    if (state)
    {
        state->setStateManager(this);
        states_.push(std::move(state));
    }
}

void StateManager::popState()
{
    if (!states_.empty())
    {
        states_.pop();
    }
}

void StateManager::changeState(std::unique_ptr<IState> state)
{
    if (!states_.empty())
    {
        states_.pop();
    }

    if (state)
    {
        state->setStateManager(this);
        states_.push(std::move(state));
    }
}

IState *StateManager::getCurrentState()
{
    if (states_.empty())
        return nullptr;
    return states_.top().get();
}

bool StateManager::isEmpty() const
{
    return states_.empty();
}

void StateManager::handleInput(sf::RenderWindow& window)
{
    if (auto *state = getCurrentState())
        state->handleInput(window);
}

void StateManager::update(float deltaTime)
{
    if (auto *state = getCurrentState())
        state->update(deltaTime);
}

void StateManager::render(sf::RenderWindow &window)
{
    if (auto *state = getCurrentState())
        state->render(window);
}