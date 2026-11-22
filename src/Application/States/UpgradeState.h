#pragma once
#include "IState.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Player;

struct UpgradeOption
{
	enum Type
	{
		NEW_WEAPON,
		UPGRADE_EXISTING
	};
	Type type;
	std::string weaponId;	 // 工厂key（新武器）
	std::string displayName; // 武器名称
	std::string textureId;	 // 纹理key
	int damage;				 // 当前伤害
	float cooldown;			 // 当前冷却
	int level;				 // 当前等级
	std::string description; // 武器描述
	int weaponIndex;		 // 玩家武器列表索引（升级已有武器时使用）
};

// 一张升级卡片：图形对象与文字对象都只建一次，之后只改内容/位置
struct UpgradeCard
{
	sf::RectangleShape frame;
	sf::RectangleShape iconBg;
	sf::Text typeText;
	sf::Text nameText;
	sf::Text descText;
	sf::Text attrText;

	explicit UpgradeCard(const sf::Font &font)
		: typeText(font), nameText(font), descText(font), attrText(font) {}
};

class UpgradeState : public IState
{
private:
	Player *player_;
	int selectedIndex_ = 0;
	std::vector<UpgradeOption> options_;
	std::vector<UpgradeCard> cards_;

	sf::RectangleShape bg_;
	sf::Font font_;
	sf::Text titleText_;

	sf::RectangleShape panelBg_;
	int langVersion_ = -1;

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
	void buildCards();
	void refreshTexts();
};
