// PauseState.cpp
#include "PauseState.h"
#include "../Statemanager.h"
#include <iostream>
#include "MenuState.h"
#include "Resourcemanager/ResourceManager.h"

PauseState::PauseState() : font_("assets/fonts/minecraft.ttf"),
						   resumeText_(font_),
						   exit_Text_(font_),
						   pauseText_(font_)
{
	std::cout << "PauseState created" << std::endl;

	if (ResourceManager::GetInstance().HasTexture("menu_button"))
	{
		sf::Texture &btnTex = ResourceManager::GetInstance().GetTexture("menu_button");
		button_resume_.setTexture(&btnTex);
		button_exit_.setTexture(&btnTex);
	}
	button_resume_.setSize({400, 50});
	button_exit_.setSize({400, 50});

	resumeText_.setString("Resume Game");
	resumeText_.setCharacterSize(28);
	resumeText_.setFillColor(sf::Color::White);

	exit_Text_.setString("Main Menu");
	exit_Text_.setCharacterSize(28);
	exit_Text_.setFillColor(sf::Color::White);

	pauseText_.setString("Pause");
	pauseText_.setCharacterSize(64);
	pauseText_.setFillColor(sf::Color::White);

	background_.setFillColor(sf::Color(0, 0, 0, 180));

	lastEscapePressed_ = false;
}

void PauseState::updateLayout(sf::Vector2u winSize)
{
	float cx = winSize.x / 2.0f;
	float cy = winSize.y / 2.0f;

	background_.setSize(sf::Vector2f(winSize.x, winSize.y));
	background_.setPosition({0, 0});

	pauseText_.setPosition({cx - pauseText_.getLocalBounds().size.x / 2, cy - 200});

	float buttonWidth = 400;
	float buttonHeight = 50;
	float verticalSpacing = 20;

	button_resume_.setSize({buttonWidth, buttonHeight});
	button_resume_.setPosition({cx - buttonWidth / 2, cy + 40});

	resumeText_.setPosition({cx - resumeText_.getLocalBounds().size.x / 2,
							 cy + 40 + buttonHeight / 2 - resumeText_.getLocalBounds().size.y / 2});

	button_exit_.setSize({buttonWidth, buttonHeight});
	button_exit_.setPosition({cx - buttonWidth / 2, cy + 40 + buttonHeight + verticalSpacing});

	exit_Text_.setPosition({cx - exit_Text_.getLocalBounds().size.x / 2,
							cy + 40 + buttonHeight + verticalSpacing + buttonHeight / 2 - exit_Text_.getLocalBounds().size.y / 2});
}

void PauseState::handleInput(sf::RenderWindow &window)
{
	sf::Vector2u winSize = window.getSize();
	window.setView(sf::View(sf::Vector2f(winSize.x / 2.f, winSize.y / 2.f), sf::Vector2f(winSize)));
	updateLayout(winSize);

	static bool wasPressed = false;

	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	sf::Vector2f mouseCoords = window.mapPixelToCoords(mousePos);

	if (button_resume_.getGlobalBounds().contains(mouseCoords))
	{
		button_resume_.setOutlineThickness(3);
		button_resume_.setOutlineColor(sf::Color::White);

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !wasPressed)
		{
			std::cout << "Resume button clicked" << std::endl;
			stateManager_->popState();
			return;
		}
	}
	else
	{
		button_resume_.setOutlineThickness(0);
	}

	if (button_exit_.getGlobalBounds().contains(mouseCoords))
	{
		button_exit_.setOutlineThickness(3);
		button_exit_.setOutlineColor(sf::Color::White);

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !wasPressed)
		{
			sf::Mouse::setPosition(sf::Vector2i(0, 0), window);
			std::cout << "Exit button clicked" << std::endl;
			stateManager_->clearAndPush(std::make_unique<MenuState>());
			return;
		}
	}
	else
	{
		button_exit_.setOutlineThickness(0);
	}
	wasPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
}

void PauseState::update(float deltaTime)
{
	(void)deltaTime;
}

void PauseState::render(sf::RenderWindow &window)
{
	sf::Vector2u winSize = window.getSize();
	window.setView(sf::View(sf::Vector2f(winSize.x / 2.f, winSize.y / 2.f), sf::Vector2f(winSize)));
	updateLayout(winSize);

	window.draw(background_);
	window.draw(pauseText_);
	window.draw(button_resume_);
	window.draw(resumeText_);
	window.draw(button_exit_);
	window.draw(exit_Text_);
}
