#include "MenuState.h"
#include <iostream>
#include "Resourcemanager/ResourceManager.h"
#include "GameState.h"
#include "../Statemanager.h"
#include "Sound/SoundManager.h"

MenuState::MenuState() : font_("assets/fonts/minecraft.ttf"), text_start_(font_), text_exit_(font_)

{
	if (ResourceManager::GetInstance().HasTexture("menu_button"))
	{
		sf::Texture &bgTex = ResourceManager::GetInstance().GetTexture("menu_button");
		start_button_.setTexture(&bgTex);
		exit_button_.setTexture(&bgTex);
	}
	if (ResourceManager::GetInstance().HasTexture("menu_title"))
	{
		sf::Texture &bgTex = ResourceManager::GetInstance().GetTexture("menu_title");
		title_.setTexture(&bgTex);
	}
	if (ResourceManager::GetInstance().HasTexture("menu_bg"))
	{
		sf::Texture &bgTex = ResourceManager::GetInstance().GetTexture("menu_bg");
		background_.setTexture(&bgTex);
	}
	else
	{
		std::cout << "Warning: menu_bg texture not found in resource manager" << std::endl;
		background_.setFillColor(sf::Color(50, 50, 100));
	}
	text_start_.setString("Start Game");
	text_exit_.setString("Exit Game");
	start_button_.setSize({400, 50});
	exit_button_.setSize({400, 50});
	title_.setSize({700, 250});

	text_start_.setCharacterSize(20);
	text_exit_.setCharacterSize(20);
	text_start_.setFillColor(sf::Color::White);
	text_exit_.setFillColor(sf::Color::White);

	// 首页bgm播放
	std::vector<std::string> playlist = {
		"assets/bgm/13 Wet Hands.mp3",
		"assets/bgm/18 Sweden.mp3",
		"assets/bgm/03 Subwoofer Lullaby.mp3"};
	SoundManager::getInstance().setBGMPlaylist(playlist);
	SoundManager::getInstance().playRandomBGM();
	SoundManager::getInstance().setBGMVolume(20);
}

void MenuState::updateLayout(sf::Vector2u winSize)
{
	float cx = winSize.x / 2.0f;
	float cy = winSize.y / 2.0f;

	background_.setSize(sf::Vector2f(winSize.x, winSize.y));
	background_.setPosition({0, 0});

	start_button_.setPosition({cx - 200, cy + 50});
	exit_button_.setPosition({cx - 200, cy + 120});
	title_.setPosition({cx - 350, winSize.y * 0.20f - 70});

	text_start_.setPosition({cx - text_start_.getLocalBounds().size.x / 2,
							 cy + 50 + 25 - text_start_.getLocalBounds().size.y / 2});
	text_exit_.setPosition({cx - text_exit_.getLocalBounds().size.x / 2,
							cy + 120 + 25 - text_exit_.getLocalBounds().size.y / 2});
}

void MenuState::handleInput(sf::RenderWindow &window)
{
	sf::Vector2u winSize = window.getSize();
	window.setView(sf::View(sf::Vector2f(winSize.x / 2.f, winSize.y / 2.f), sf::Vector2f(winSize)));
	updateLayout(winSize);

	static bool wasPressed = false;

	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	sf::Vector2f mouseCoords = window.mapPixelToCoords(mousePos);

	bool leftPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

	if (start_button_.getGlobalBounds().contains(mouseCoords))
	{
		start_button_.setOutlineThickness(3);
		start_button_.setOutlineColor(sf::Color::White);

		if (leftPressed && !wasPressed)
		{
			std::cout << "Start Game" << std::endl;
			stateManager_->changeState(std::make_unique<GameState>());
			sf::sleep(sf::milliseconds(100));
			return;
		}
	}
	else
	{
		start_button_.setOutlineThickness(0);
	}

	if (exit_button_.getGlobalBounds().contains(mouseCoords))
	{
		exit_button_.setOutlineThickness(3);
		exit_button_.setOutlineColor(sf::Color::White);

		if (leftPressed && !wasPressed)
		{
			std::cout << "Exit Game" << std::endl;
			exit(0);
		}
	}
	else
	{
		exit_button_.setOutlineThickness(0);
	}

	wasPressed = leftPressed;
}

void MenuState::update(float deltaTime)
{
	(void)deltaTime;
}

void MenuState::render(sf::RenderWindow &window)
{
	sf::Vector2u winSize = window.getSize();
	window.setView(sf::View(sf::Vector2f(winSize.x / 2.f, winSize.y / 2.f), sf::Vector2f(winSize)));
	updateLayout(winSize);

	window.draw(background_);
	window.draw(start_button_);
	window.draw(exit_button_);
	window.draw(title_);
	window.draw(text_start_);
	window.draw(text_exit_);
}
