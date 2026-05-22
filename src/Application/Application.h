#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include "StateManager.h"

/**
 * @brief
 */
class Application
{
private:
    sf::RenderWindow window_;      
    StateManager stateManager_;   
    bool isRunning_;               
    sf::Clock clock_;              

public:
    Application();                 
    void run();                    
private:
    void handleGlobalEvents();     
};
