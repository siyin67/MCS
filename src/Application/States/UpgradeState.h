#pragma once
#include "IState.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Player;

struct UpgradeOption
{
	enum Type { NEW_WEAPON, UPGRADE_EXISTING };
	Type type;
	std::string weaponId;     // 工厂key（新武器）
	std::string displayName;  // 武器名称
	std::string textureId;    // 纹理key
	int damage;               // 当前伤害
	float cooldown;           // 当前冷却
	int level;                // 当前等级
	std::string description;  // 武器描述
	int weaponIndex;           // 玩家武器列表索引（升级已有武器时使用）
};

class UpgradeState : public IState
{
private:
	Player *player_;
	int selectedIndex_;
	std::vector<UpgradeOption> options_;

	sf::RectangleShape bg_;
	sf::Font font_;
	sf::Text titleText_;

	sf::RectangleShape panelBg_;
	sf::RectangleShape iconFrame_;
	sf::RectangleShape icon_;

public:
	UpgradeState(Player *player);
	~UpgradeState() = default;

	void handleInput(sf::RenderWindow &window) override;
	void update(float deltaTime) override;
	void render(sf::RenderWindow &window) override;

private:
	void generateOptions();
	void applyOption(int index);
	void skip();
	void updateLayout(sf::Vector2u winSize);
};
