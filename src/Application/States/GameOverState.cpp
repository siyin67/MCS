#include "GameOverState.h"
#include "../StateManager.h"
#include "MenuState.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include <iostream>

GameOverState::GameOverState()
	: font_("assets/fonts/minecraft.ttf"),
	  gameOverText_(font_), buttonText_(font_)
{
	std::cout << "GameOverState created" << std::endl;

	background_.setFillColor(sf::Color(100, 0, 0, 180));

	if (ResourceManager::GetInstance().HasTexture("menu_button"))
	{
		sf::Texture &btnTex = ResourceManager::GetInstance().GetTexture("menu_button");
		button_.setTexture(&btnTex);
	}
	button_.setSize({400, 50});

	gameOverText_.setString("GAME OVER");
	gameOverText_.setCharacterSize(64);
	gameOverText_.setFillColor(sf::Color::White);

	buttonText_.setString("Main Menu");
	buttonText_.setCharacterSize(28);
	buttonText_.setFillColor(sf::Color::White);
	SoundManager::getInstance().stopBGM();
	SoundManager::getInstance().setSFXVolume(20);
	SoundManager::getInstance().playSFX("gameover");
}

void GameOverState::updateLayout(sf::Vector2u winSize)
{
	float cx = winSize.x / 2.0f;
	float cy = winSize.y / 2.0f;

	background_.setSize(sf::Vector2f(winSize.x, winSize.y));
	background_.setPosition({0, 0});

	gameOverText_.setPosition({cx - gameOverText_.getLocalBounds().size.x / 2, cy - 200});

	float buttonWidth = 400;
	float buttonHeight = 50;
	button_.setSize({buttonWidth, buttonHeight});
	button_.setPosition({cx - buttonWidth / 2, cy + 40});

	buttonText_.setPosition({cx - buttonText_.getLocalBounds().size.x / 2,
							 cy + 40 + buttonHeight / 2 - buttonText_.getLocalBounds().size.y / 2});
}

void GameOverState::handleInput(sf::RenderWindow &window)
{
	sf::Vector2u winSize = window.getSize();
	window.setView(sf::View(sf::Vector2f(winSize.x / 2.f, winSize.y / 2.f), sf::Vector2f(winSize)));
	updateLayout(winSize);

	static bool wasPressed = false;

	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	sf::Vector2f mouseCoords = window.mapPixelToCoords(mousePos);

	if (button_.getGlobalBounds().contains(mouseCoords))
	{
		button_.setOutlineThickness(3);
		button_.setOutlineColor(sf::Color::White);

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !wasPressed)
		{
			sf::Mouse::setPosition(sf::Vector2i(0, 0), window);
			std::cout << "Button clicked, back to menu" << std::endl;
			stateManager_->clearAndPush(std::make_unique<MenuState>());
			return;
		}
	}
	else
	{
		button_.setOutlineThickness(0);
	}
	wasPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
}

void GameOverState::update(float deltaTime)
{
	(void)deltaTime;
}

void GameOverState::render(sf::RenderWindow &window)
{
	sf::Vector2u winSize = window.getSize();
	window.setView(sf::View(sf::Vector2f(winSize.x / 2.f, winSize.y / 2.f), sf::Vector2f(winSize)));
	updateLayout(winSize);

	window.draw(background_);
	window.draw(gameOverText_);
	window.draw(button_);
	window.draw(buttonText_);
}
