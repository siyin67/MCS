#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Player;
class Weapon;

class UIManager
{
private:
	int fullHearts_ = 10;
	int halfHearts_ = 0;
	int emptyHearts_ = 0;

	std::unique_ptr<sf::Sprite> hotbarSprite_;
	float slotWidth_ = 0.0f;
	float slotHeight_ = 0.0f;

	sf::Font font_;

	// 游戏时间
	float gameTime_;
	sf::Text gameTimeText_;

	// 经验条
	float xpPercent_ = 0.0f;
	int playerLevel_ = 1;
	sf::Text xpLevelText_;

	// 当前持有的武器列表
	std::vector<Weapon *> weapons_;

	static constexpr int MAX_HEARTS = 10;
	static constexpr float HEART_SPACING = 28.0f;
	static constexpr int HOTBAR_SLOTS = 9;

public:
	UIManager();

	void update(const Player &player, float deltaTime);
	void render(sf::RenderWindow &window);
};