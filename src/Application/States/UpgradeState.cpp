#include "UpgradeState.h"
#include "../StateManager.h"
#include "Localization/Localization.h"
#include "Localization/UiText.h"
#include "Player/Player.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include "Weapon/WeaponFactory.h"
#include "Weapon/WeaponManager.h"
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <random>

namespace
{
constexpr float PANEL_W = 1000.0f;
constexpr float PANEL_H = 700.0f;
constexpr float CARD_W = 280.0f;
constexpr float CARD_H = 320.0f;
constexpr float CARD_GAP = 30.0f;
constexpr float CARD_TOP = 30.0f; // 卡片相对面板顶部的偏移
constexpr float ICON_BOX = 96.0f;
constexpr float ICON_SIZE = 80.0f;
constexpr float HEADER_H = 110.0f; // 面板顶部留给标题的高度

std::string formatSeconds(float value)
{
	char buf[32];
	std::snprintf(buf, sizeof(buf), "%.1f", value);
	return buf;
}

// 把文字水平居中到 x=cx、顶端对齐 y（考虑字形包围盒的偏移）
void placeAt(sf::Text &text, float cx, float y)
{
	const sf::FloatRect b = text.getLocalBounds();
	text.setPosition({cx - b.size.x / 2.0f - b.position.x, y - b.position.y});
}

// 语言文件里没有对应 key 时的兜底描述（英文，来自旧实现）
std::string fallbackDesc(const std::string &name)
{
	if (name == "Iron Sword") return "Swing in a wide arc";
	if (name == "Bow") return "Shoot arrows from distance";
	if (name == "Shield") return "Orbiting shields block enemies";
	if (name == "TNT") return "Throw explosive TNT";
	if (name == "Trident") return "Piercing auto-aim trident";
	if (name == "Clock") return "Reset cooldowns, freeze at Lv10";
	if (name == "Axe") return "Throw spinning axe, area damage";
	if (name == "Dragon Breath") return "Create damaging breath circle";
	return "";
}
} // namespace

UpgradeState::UpgradeState(Player *player)
	: player_(player), font_("assets/fonts/minecraft.ttf"), titleText_(font_)
{
	std::cout << "UpgradeState created" << std::endl;

	bg_.setFillColor(sf::Color(0, 0, 0, 180));

	if (ResourceManager::GetInstance().HasTexture("upgrade_background"))
	{
		panelBg_.setTexture(&ResourceManager::GetInstance().GetTexture("upgrade_background"));
	}
	panelBg_.setFillColor(sf::Color::White);

	// 面板是浅灰底图，白色字会糊成一片，标题用深灰
	titleText_.setCharacterSize(64);
	titleText_.setFillColor(sf::Color(45, 45, 45));

	generateOptions();
	buildCards();
	refreshTexts();
}

void UpgradeState::generateOptions()
{
	options_.clear();

	auto &weapons = player_->getWeaponManager().getWeapons();
	auto allIds = WeaponFactory::getInstance().getAllWeaponIds();

	// 找出玩家未拥有的武器（槽满了就不再提供新武器）
	std::vector<std::string> newWeaponIds;
	if ((int)weapons.size() < player_->getWeaponManager().getMaxWeapons())
	{
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
		opt.displayName = trOr("weapon." + id + ".name", temp->getName());
		opt.textureId = temp->getTextureId();
		opt.damage = temp->getDamage();
		opt.cooldown = temp->getAttackCooldown();
		opt.level = 1;
		opt.description = trOr("weapon." + id + ".desc", fallbackDesc(temp->getName()));
		opt.weaponIndex = -1;
		pool.push_back(opt);
	}

	// 升级已有武器选项
	for (int i = 0; i < (int)weapons.size(); i++)
	{
		if (!weapons[i]) continue;
		UpgradeOption opt;
		opt.type = UpgradeOption::UPGRADE_EXISTING;
		opt.weaponId = weapons[i]->getId();
		opt.displayName = trOr("weapon." + opt.weaponId + ".name", weapons[i]->getName());
		opt.textureId = weapons[i]->getTextureId();
		opt.damage = weapons[i]->getDamage();
		opt.cooldown = weapons[i]->getAttackCooldown();
		opt.level = weapons[i]->getLevel();
		opt.description = trOr("weapon." + opt.weaponId + ".desc", fallbackDesc(weapons[i]->getName()));
		opt.weaponIndex = i;
		pool.push_back(opt);
	}

	// 随机选3个
	std::random_device rd;
	std::mt19937 rng(rd());

	int weaponCount = (int)weapons.size();
	if (weaponCount < 3)
	{
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
		{
			if (options_.size() >= 3) break;
			options_.push_back(opt);
		}
		for (auto &opt : upgOpts)
		{
			if (options_.size() >= 3) break;
			options_.push_back(opt);
		}
		while (options_.size() < 3 && !newOpts.empty())
		{
			options_.push_back(newOpts[options_.size() % newOpts.size()]);
		}
	}
	else
	{
		std::shuffle(pool.begin(), pool.end(), rng);
		for (int i = 0; i < 3 && i < (int)pool.size(); i++)
			options_.push_back(pool[i]);
	}

	std::cout << "Generated " << options_.size() << " upgrade options" << std::endl;
}

void UpgradeState::buildCards()
{
	cards_.clear();
	cards_.reserve(options_.size());

	for (size_t i = 0; i < options_.size(); ++i)
	{
		cards_.emplace_back(font_);
		UpgradeCard &card = cards_.back();

		card.frame.setFillColor(sf::Color(0, 0, 0, 16));
		card.frame.setOutlineThickness(2.0f);
		card.frame.setOutlineColor(sf::Color(90, 90, 90, 150));

		if (ResourceManager::GetInstance().HasTexture("upgradeitem_background"))
		{
			card.iconBg.setTexture(&ResourceManager::GetInstance().GetTexture("upgradeitem_background"));
		}
		card.iconBg.setFillColor(sf::Color::White);

		card.typeText.setCharacterSize(16);
		card.nameText.setCharacterSize(24);
		card.nameText.setFillColor(sf::Color(30, 30, 30));
		card.descText.setCharacterSize(16);
		card.descText.setFillColor(sf::Color(60, 60, 60));
		card.attrText.setCharacterSize(14);
		card.attrText.setFillColor(sf::Color(95, 95, 95));
	}
}

void UpgradeState::refreshTexts()
{
	langVersion_ = Localization::getInstance().version();

	ui_text::set(titleText_, tr("upgrade.title"));

	for (size_t i = 0; i < cards_.size() && i < options_.size(); ++i)
	{
		const UpgradeOption &opt = options_[i];
		UpgradeCard &card = cards_[i];

		const bool isNew = opt.type == UpgradeOption::NEW_WEAPON;
		ui_text::set(card.typeText, tr(isNew ? "upgrade.type_new" : "upgrade.type_upgrade"));
		card.typeText.setFillColor(isNew ? sf::Color(0, 120, 0) : sf::Color(165, 105, 0));

		ui_text::set(card.nameText, opt.displayName);
		ui_text::set(card.descText, opt.description);

		const std::string stats = trf("upgrade.stats", {std::to_string(opt.damage),
														formatSeconds(opt.cooldown)});
		if (isNew)
		{
			ui_text::set(card.attrText, tr("upgrade.level_new") + "  " + stats);
		}
		else
		{
			ui_text::set(card.attrText, trf("upgrade.level_from_to", {std::to_string(opt.level),
																	  std::to_string(opt.level + 1)}) +
											"  " + stats);
		}
	}
}

void UpgradeState::updateLayout(sf::Vector2u winSize)
{
	const float cx = winSize.x / 2.0f;
	const float cy = winSize.y / 2.0f;

	bg_.setSize({static_cast<float>(winSize.x), static_cast<float>(winSize.y)});

	panelBg_.setSize({PANEL_W, PANEL_H});
	panelBg_.setPosition({cx - PANEL_W / 2.0f, cy - PANEL_H / 2.0f});

	placeAt(titleText_, cx, cy - PANEL_H / 2.0f + 44.0f);

	const int count = static_cast<int>(cards_.size());
	if (count == 0)
	{
		return;
	}

	const float totalW = count * CARD_W + (count - 1) * CARD_GAP;
	const float startX = cx - totalW / 2.0f;
	// 卡片在标题下面那部分区域里垂直居中，别全挤在面板上半边
	const float cardY = cy - PANEL_H / 2.0f + HEADER_H +
						(PANEL_H - HEADER_H - CARD_H) / 2.0f;

	for (int i = 0; i < count; ++i)
	{
		UpgradeCard &card = cards_[i];
		const float x = startX + i * (CARD_W + CARD_GAP);
		const float cardCx = x + CARD_W / 2.0f;

		card.frame.setSize({CARD_W, CARD_H});
		card.frame.setPosition({x, cardY});

		card.iconBg.setSize({ICON_BOX, ICON_BOX});
		card.iconBg.setPosition({cardCx - ICON_BOX / 2.0f, cardY + 24.0f});

		placeAt(card.typeText, cardCx, cardY + 4.0f);
		placeAt(card.nameText, cardCx, cardY + 130.0f);
		placeAt(card.descText, cardCx, cardY + 172.0f);
		placeAt(card.attrText, cardCx, cardY + 214.0f);
	}
}

void UpgradeState::handleInput(sf::RenderWindow &window)
{
	sf::Vector2u winSize = window.getSize();
	window.setView(sf::View(sf::Vector2f(winSize.x / 2.f, winSize.y / 2.f), sf::Vector2f(winSize)));
	updateLayout(winSize);

	static bool wasPressed = false;

	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	sf::Vector2f mouseCoords = window.mapPixelToCoords(mousePos);

	for (int i = 0; i < (int)cards_.size(); i++)
	{
		if (cards_[i].frame.getGlobalBounds().contains(mouseCoords))
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
	SoundManager::getInstance().playSFX("upgrade_select");
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
	SoundManager::getInstance().playSFX("ui_click");
	std::cout << "Upgrade skipped" << std::endl;
	stateManager_->popState();
}

void UpgradeState::update(float deltaTime)
{
	(void)deltaTime;
}

void UpgradeState::render(sf::RenderWindow &window)
{
	sf::Vector2u winSize = window.getSize();
	window.setView(sf::View(sf::Vector2f(winSize.x / 2.f, winSize.y / 2.f), sf::Vector2f(winSize)));

	if (langVersion_ != Localization::getInstance().version())
	{
		refreshTexts();
	}
	updateLayout(winSize);

	window.draw(bg_);
	window.draw(panelBg_);
	window.draw(titleText_);

	for (size_t i = 0; i < cards_.size() && i < options_.size(); ++i)
	{
		UpgradeCard &card = cards_[i];
		const UpgradeOption &opt = options_[i];

		const bool selected = static_cast<int>(i) == selectedIndex_;
		card.frame.setOutlineThickness(selected ? 4.0f : 2.0f);
		card.frame.setOutlineColor(selected ? sf::Color::Yellow : sf::Color(90, 90, 90, 150));
		window.draw(card.frame);
		window.draw(card.iconBg);

		// 武器图标：等比缩放到 80x80 的框里并居中，不再拉成正方形
		if (!opt.textureId.empty() && ResourceManager::GetInstance().HasTexture(opt.textureId))
		{
			sf::Texture &tex = ResourceManager::GetInstance().GetTexture(opt.textureId);
			const sf::Vector2u texSize = tex.getSize();
			if (texSize.x > 0 && texSize.y > 0)
			{
				sf::Sprite icon(tex);
				const float scale = ICON_SIZE / static_cast<float>(std::max(texSize.x, texSize.y));
				icon.setScale({scale, scale});

				const sf::FloatRect bounds = icon.getLocalBounds();
				const sf::Vector2f boxCenter = card.iconBg.getPosition() + sf::Vector2f(ICON_BOX / 2.0f, ICON_BOX / 2.0f);
				icon.setPosition({boxCenter.x - (bounds.position.x + bounds.size.x / 2.0f) * scale,
								  boxCenter.y - (bounds.position.y + bounds.size.y / 2.0f) * scale});
				window.draw(icon);
			}
		}

		window.draw(card.typeText);
		window.draw(card.nameText);
		window.draw(card.descText);
		window.draw(card.attrText);
	}
}
