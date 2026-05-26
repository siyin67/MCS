#include "Application.h"
#include "Resourcemanager/ResourceManager.h"
#include "States/MenuState.h"
#include "States/GameState.h"

Application::Application()
    : isRunning_(true)
{
    ResourceManager::GetInstance().LoadFromFile("assets/resources.txt");
    window_.create(sf::VideoMode({1280, 720}), "MinecraftSurvivor");
    window_.setMaximumSize(sf::Vector2u(2560, 1440));
    window_.setMinimumSize(sf::Vector2u(1280, 720));
    window_.setFramerateLimit(60);

    stateManager_.pushState(std::make_unique<MenuState>());
}

void Application::run()
{
    std::cout << "Application::run started" << std::endl;
    while (window_.isOpen() && isRunning_)
    {
        float deltaTime = clock_.restart().asSeconds();

        handleGlobalEvents();

        stateManager_.handleInput(window_);

        stateManager_.update(deltaTime);

        window_.clear();
        stateManager_.render(window_);
        window_.display();
    }
    std::cout << "Application::run ended" << std::endl;
}

void Application::handleGlobalEvents()
{
    while (const auto event = window_.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window_.close();
            isRunning_ = false;
        }
    }
}