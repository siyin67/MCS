#include "UpgradeState.h"
#include "../StateManager.h"
#include "Player/Player.h"
#include "Resourcemanager/ResourceManager.h"
#include "Weapon/WeaponFactory.h"
#include "Weapon/WeaponManager.h"
#include <iostream>
#include <algorithm>
#include <random>

UpgradeState::UpgradeState(Player *player)
	: player_(player), selectedIndex_(0), font_("assets/fonts/minecraft.ttf"), titleText_(font_)
{
	std::cout << "UpgradeState created" << std::endl;

	bg_.setFillColor(sf::Color(0, 0, 0, 180));

	if (ResourceManager::GetInstance().HasTexture("upgrade_background"))
	{
		panelBg_.setTexture(&ResourceManager::GetInstance().GetTexture("upgrade_background"));
	}
	panelBg_.setFillColor(sf::Color::White);

	if (ResourceManager::GetInstance().HasTexture("upgradeitem_background"))
	{
		iconFrame_.setTexture(&ResourceManager::GetInstance().GetTexture("upgradeitem_background"));
	}
	iconFrame_.setFillColor(sf::Color::White);

	icon_.setFillColor(sf::Color::White);

	titleText_.setString("LEVEL UP!");
	titleText_.setCharacterSize(64);
	titleText_.setFillColor(sf::Color::White);

	generateOptions();
}

static std::string getWeaponDesc(const std::string &name)
{
	if (name == "Iron Sword") return "Swing in a wide arc";
	if (name == "Bow") return "Shoot arrows from distance";
	if (name == "Shield") return "Orbiting shields block enemies";
	if (name == "TNT") return "Throw explosive TNT";
	if (name == "Trident") return "Piercing auto-aim trident";
	return "";
}

void UpgradeState::generateOptions()
{
	options_.clear();

	auto &weapons = player_->getWeaponManager().getWeapons();
	auto allIds = WeaponFactory::getInstance().getAllWeaponIds();

	// 找出玩家未拥有的武器
	std::vector<std::string> newWeaponIds;
	for (auto &id : allIds)
	{
		auto temp = WeaponFactory::getInstance().create(id);
		if (!temp) continue;
		std::string name = temp->getName();
		bool owned = false;
		for (auto &w : weapons)
		{
			if (w && w->getName() == name)
			{
				owned = true;
				break;
			}
		}
		if (!owned)
			newWeaponIds.push_back(id);
	}

	// 构建可选池
	std::vector<UpgradeOption> pool;

	// 新武器选项
	for (auto &id : newWeaponIds)
	{
		auto temp = WeaponFactory::getInstance().create(id);
		if (!temp) continue;
		UpgradeOption opt;
		opt.type = UpgradeOption::NEW_WEAPON;
		opt.weaponId = id;
		opt.displayName = temp->getName();
		opt.textureId = temp->getTextureId();
		opt.damage = temp->getDamage();
		opt.cooldown = temp->getAttackCooldown();
		opt.level = 1;
		opt.description = getWeaponDesc(temp->getName());
		opt.weaponIndex = -1;
		pool.push_back(opt);
	}

	// 升级已有武器选项
	for (int i = 0; i < (int)weapons.size(); i++)
	{
		if (!weapons[i]) continue;
		UpgradeOption opt;
		opt.type = UpgradeOption::UPGRADE_EXISTING;
		opt.weaponId = "";
		opt.displayName = weapons[i]->getName();
		opt.textureId = weapons[i]->getTextureId();
		opt.damage = weapons[i]->getDamage();
		opt.cooldown = weapons[i]->getAttackCooldown();
		opt.level = weapons[i]->getLevel();
		opt.description = getWeaponDesc(weapons[i]->getName());
		opt.weaponIndex = i;
		pool.push_back(opt);
	}

	// 随机选3个
	std::random_device rd;
	std::mt19937 rng(rd());

	int weaponCount = (int)weapons.size();
	if (weaponCount < 3)
	{
		// 优先新武器，不够再用升级补
		std::vector<UpgradeOption> newOpts, upgOpts;
		for (auto &opt : pool)
		{
			if (opt.type == UpgradeOption::NEW_WEAPON)
				newOpts.push_back(opt);
			else
				upgOpts.push_back(opt);
		}
		std::shuffle(newOpts.begin(), newOpts.end(), rng);
		std::shuffle(upgOpts.begin(), upgOpts.end(), rng);

		for (auto &opt : newOpts)
			options_.push_back(opt);
		for (auto &opt : upgOpts)
		{
			if (options_.size() >= 3) break;
			options_.push_back(opt);
		}
		// 仍然不足3个（极端情况），补空选项
		while (options_.size() < 3 && !newOpts.empty())
		{
			options_.push_back(newOpts[options_.size() % newOpts.size()]);
		}
	}
	else
	{
		// 武器>=3，混合池随机选3个
		std::shuffle(pool.begin(), pool.end(), rng);
		for (int i = 0; i < 3 && i < (int)pool.size(); i++)
			options_.push_back(pool[i]);
	}

	std::cout << "Generated " << options_.size() << " upgrade options" << std::endl;
}

void UpgradeState::handleInput(sf::RenderWindow &window)
{
	static bool wasPressed = false;

	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	float mouseX = static_cast<float>(mousePos.x);
	float mouseY = static_cast<float>(mousePos.y);

	sf::Vector2u windowSize = window.getSize();
	float centerX = windowSize.x / 2.0f;
	float buttonWidth = 280;
	float buttonHeight = 320;
	float spacing = 30;
	float startX = centerX - (buttonWidth * 3 + spacing * 2) / 2;
	float buttonY = windowSize.y / 2.0f - buttonHeight / 2 + 30;

	for (int i = 0; i < (int)options_.size(); i++)
	{
		float buttonX = startX + i * (buttonWidth + spacing);
		sf::FloatRect buttonRect({buttonX, buttonY}, {buttonWidth, buttonHeight});

		if (buttonRect.contains({mouseX, mouseY}))
		{
			selectedIndex_ = i;
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !wasPressed)
			{
				applyOption(i);
				return;
			}
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1) && !wasPressed)
		applyOption(0);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2) && !wasPressed)
		applyOption(1);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3) && !wasPressed)
		applyOption(2);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4) && !wasPressed)
		skip();
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num5) && !wasPressed)
		skip();
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) && !wasPressed)
		skip();

	wasPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) ||
				 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1) ||
				 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2) ||
				 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3) ||
				 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4) ||
				 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num5) ||
				 sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
}

void UpgradeState::applyOption(int index)
{
	if (index < 0 || index >= (int)options_.size())
	{
		stateManager_->popState();
		return;
	}

	auto &opt = options_[index];
	if (opt.type == UpgradeOption::NEW_WEAPON)
	{
		auto weapon = WeaponFactory::getInstance().create(opt.weaponId);
		if (weapon)
		{
			std::cout << "Adding new weapon: " << opt.displayName << std::endl;
			player_->addWeapon(std::move(weapon));
		}
	}
	else if (opt.type == UpgradeOption::UPGRADE_EXISTING)
	{
		std::cout << "Upgrading weapon: " << opt.displayName
				  << " (Lv." << opt.level << " -> " << (opt.level + 1) << ")" << std::endl;
		player_->getWeaponManager().upgradeWeapon(opt.weaponIndex);
	}

	stateManager_->popState();
}

void UpgradeState::skip()
{
	std::cout << "Upgrade skipped" << std::endl;
	stateManager_->popState();
}

void UpgradeState::update(float deltaTime)
{
	(void)deltaTime;
}

void UpgradeState::render(sf::RenderWindow &window)
{
	window.setView(window.getDefaultView());

	sf::Vector2u windowSize = window.getSize();
	float centerX = windowSize.x / 2.0f;
	float centerY = windowSize.y / 2.0f;

	// 半透明遮罩
	bg_.setSize({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
	window.draw(bg_);

	// 升级界面背景面板
	float panelW = 1000;
	float panelH = 700;
	float panelX = centerX - panelW / 2;
	float panelY = centerY - panelH / 2;
	panelBg_.setSize({panelW, panelH});
	panelBg_.setPosition({panelX, panelY});
	window.draw(panelBg_);

	// 标题
	sf::FloatRect titleBounds = titleText_.getLocalBounds();
	titleText_.setPosition({centerX - titleBounds.size.x / 2, centerY - 300});
	window.draw(titleText_);

	// 三个按钮区域
	float buttonWidth = 280;
	float buttonHeight = 320;
	float spacing = 30;
	float startX = centerX - (buttonWidth * 3 + spacing * 2) / 2;
	float buttonY = centerY - buttonHeight / 2 + 30;

	for (int i = 0; i < (int)options_.size(); i++)
	{
		auto &opt = options_[i];
		float buttonX = startX + i * (buttonWidth + spacing);

		// 图标框（96x96）
		float frameSize = 96;
		float frameX = buttonX + (buttonWidth - frameSize) / 2;
		float frameY = buttonY + 25;
		iconFrame_.setSize({frameSize, frameSize});
		iconFrame_.setPosition({frameX, frameY});
		window.draw(iconFrame_);

		// 武器图标（80x80）
		float iconSize = 80;
		float iconX = frameX + (frameSize - iconSize) / 2;
		float iconY = frameY + (frameSize - iconSize) / 2;
		icon_.setSize({iconSize, iconSize});
		icon_.setPosition({iconX, iconY});
		if (ResourceManager::GetInstance().HasTexture(opt.textureId))
		{
			icon_.setTexture(&ResourceManager::GetInstance().GetTexture(opt.textureId));
		}
		window.draw(icon_);

		// 类型标签（NEW 或 UPGRADE）
		sf::Text typeText(font_);
		typeText.setString(opt.type == UpgradeOption::NEW_WEAPON ? "NEW" : "UPGRADE");
		typeText.setCharacterSize(14);
		typeText.setFillColor(opt.type == UpgradeOption::NEW_WEAPON
								  ? sf::Color(100, 255, 100)
								  : sf::Color(255, 215, 0));
		sf::FloatRect typeBounds = typeText.getLocalBounds();
		typeText.setPosition({buttonX + buttonWidth / 2 - typeBounds.size.x / 2, buttonY + 5});
		window.draw(typeText);

		// 武器名称
		sf::Text nameText(font_);
		nameText.setString(opt.displayName);
		nameText.setCharacterSize(22);
		nameText.setFillColor(sf::Color::White);
		sf::FloatRect nameBounds = nameText.getLocalBounds();
		nameText.setPosition({buttonX + buttonWidth / 2 - nameBounds.size.x / 2, buttonY + 145});
		window.draw(nameText);

		// 武器描述
		sf::Text descText(font_);
		descText.setString(opt.description);
		descText.setCharacterSize(16);
		descText.setFillColor(sf::Color(200, 200, 200));
		sf::FloatRect descBounds = descText.getLocalBounds();
		descText.setPosition({buttonX + buttonWidth / 2 - descBounds.size.x / 2, buttonY + 185});
		window.draw(descText);

		// 等级属性文字
		sf::Text attrText(font_);
		if (opt.type == UpgradeOption::UPGRADE_EXISTING)
		{
			attrText.setString("Lv." + std::to_string(opt.level) + " -> " +
							   std::to_string(opt.level + 1) +
							   "  Dmg:" + std::to_string(opt.damage) +
							   "  CD:" + std::to_string(opt.cooldown).substr(0, 3) + "s");
		}
		else
		{
			attrText.setString("Lv.1  Dmg:" + std::to_string(opt.damage) +
							   "  CD:" + std::to_string(opt.cooldown).substr(0, 3) + "s");
		}
		attrText.setCharacterSize(14);
		attrText.setFillColor(sf::Color(150, 150, 150));
		sf::FloatRect attrBounds = attrText.getLocalBounds();
		attrText.setPosition({buttonX + buttonWidth / 2 - attrBounds.size.x / 2, buttonY + 215});
		window.draw(attrText);

		// 边框高亮
		sf::RectangleShape outline;
		outline.setSize({buttonWidth, buttonHeight});
		outline.setPosition({buttonX, buttonY});
		outline.setFillColor(sf::Color::Transparent);
		outline.setOutlineThickness(i == selectedIndex_ ? 4 : 2);
		outline.setOutlineColor(i == selectedIndex_ ? sf::Color::Yellow : sf::Color(80, 80, 80, 150));
		window.draw(outline);
	}
}