#include "UIManager.h"
#include "../Player/player.h"
#include "../Weapon/weapon.h"
#include "../Resourcemanager/ResourceManager.h"
#include <iomanip>
#include <sstream>

UIManager::UIManager()
    : font_("assets/fonts/minecraft.ttf"), gameTime_(0.0f), gameTimeText_(font_), xpLevelText_(font_)
{
    gameTimeText_.setCharacterSize(28);
    gameTimeText_.setFillColor(sf::Color::White);

    xpLevelText_.setCharacterSize(14);
    xpLevelText_.setFillColor(sf::Color::White);
    xpLevelText_.setOutlineColor(sf::Color::Black);
    xpLevelText_.setOutlineThickness(1.5f);

    if (ResourceManager::GetInstance().HasTexture("hotbar"))
    {
        sf::Texture &tex = ResourceManager::GetInstance().GetTexture("hotbar");
        hotbarSprite_ = std::make_unique<sf::Sprite>(tex);
        slotWidth_ = tex.getSize().x / static_cast<float>(HOTBAR_SLOTS);
        slotHeight_ = static_cast<float>(tex.getSize().y);
    }
}

void UIManager::update(const Player &player, float deltaTime)
{
    gameTime_ += deltaTime;

    int health = player.getHealth();
    fullHearts_ = health / 2;
    halfHearts_ = (health % 2) == 1 ? 1 : 0;
    emptyHearts_ = MAX_HEARTS - fullHearts_ - halfHearts_;

    xpPercent_ = player.getXPPercent();
    playerLevel_ = player.getLevel();

    weapons_.clear();
    for (const auto &w : player.getWeaponManager().getWeapons())
    {
        weapons_.push_back(w.get());
    }

    // 格式化游戏时间 MM:SS
    int totalSec = static_cast<int>(gameTime_);
    int minutes = totalSec / 60;
    int seconds = totalSec % 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << minutes << ":"
        << std::setw(2) << std::setfill('0') << seconds;
    gameTimeText_.setString(oss.str());
}

void UIManager::render(sf::RenderWindow &window)
{
    sf::Vector2f screenSize(static_cast<float>(window.getSize().x),
                            static_cast<float>(window.getSize().y));

    static constexpr float HEART_SCALE = 2.2f;
    static constexpr float HOTBAR_SCALE = 2.2f;
    static constexpr float HEART_GAP = 2.0f;
    static constexpr float HOTBAR_BOTTOM_MARGIN = 10.0f;
    static constexpr float XP_BAR_HEIGHT = 12.0f;

    if (!hotbarSprite_)
        return;

    // ========== 1. 游戏时间（屏幕最上方居中） ==========
    sf::FloatRect timeBounds = gameTimeText_.getLocalBounds();
    gameTimeText_.setPosition({(screenSize.x - timeBounds.size.x) / 2.0f, 8.0f});
    window.draw(gameTimeText_);

    // ========== 2. 计算物品栏位置 ==========
    hotbarSprite_->setScale({HOTBAR_SCALE, HOTBAR_SCALE});
    sf::FloatRect hb = hotbarSprite_->getLocalBounds();
    float hotbarW = hb.size.x * HOTBAR_SCALE;
    float hotbarH = hb.size.y * HOTBAR_SCALE;
    float hotbarX = (screenSize.x - hotbarW) / 2.0f;
    float hotbarY = screenSize.y - hotbarH - HOTBAR_BOTTOM_MARGIN;

    // ========== 3. 计算红心尺寸 ==========
    sf::Vector2f heartSize(14.0f, 14.0f);
    if (ResourceManager::GetInstance().HasTexture("heart_full"))
    {
        auto &ht = ResourceManager::GetInstance().GetTexture("heart_full");
        heartSize = {ht.getSize().x * HEART_SCALE, ht.getSize().y * HEART_SCALE};
    }
    float heartSpacing = heartSize.x + HEART_GAP;

    // ========== 4. 红心血条 ==========
    float xpBarY = hotbarY - XP_BAR_HEIGHT - 6.0f;
    float heartY = xpBarY - heartSize.y - 4.0f;
    float heartX = hotbarX;

    auto drawHeart = [&](const std::string &texId, float x, float y)
    {
        if (ResourceManager::GetInstance().HasTexture(texId))
        {
            sf::Sprite sprite(ResourceManager::GetInstance().GetTexture(texId));
            sprite.setScale({HEART_SCALE, HEART_SCALE});
            sprite.setPosition({x, y});
            window.draw(sprite);
        }
    };

    for (int i = 0; i < fullHearts_; ++i)
    {
        drawHeart("heart_full", heartX, heartY);
        heartX += heartSpacing;
    }
    for (int i = 0; i < halfHearts_; ++i)
    {
        drawHeart("heart_half", heartX, heartY);
        heartX += heartSpacing;
    }
    for (int i = 0; i < emptyHearts_; ++i)
    {
        drawHeart("heart_empty", heartX, heartY);
        heartX += heartSpacing;
    }

    // ========== 5. 经验条（红心下方，物品栏上方）==========
    if (ResourceManager::GetInstance().HasTexture("xpbar_empty") &&
        ResourceManager::GetInstance().HasTexture("xpbar_full"))
    {
        sf::Texture &xpEmptyTex = ResourceManager::GetInstance().GetTexture("xpbar_empty");
        sf::Texture &xpFullTex = ResourceManager::GetInstance().GetTexture("xpbar_full");

        float xpBarWidth = hotbarW;
        sf::Sprite xpEmpty(xpEmptyTex);
        float xpScaleX = xpBarWidth / xpEmptyTex.getSize().x;
        float xpScaleY = XP_BAR_HEIGHT / xpEmptyTex.getSize().y;
        xpEmpty.setScale({xpScaleX, xpScaleY});
        xpEmpty.setPosition({hotbarX, xpBarY});
        window.draw(xpEmpty);

        sf::Sprite xpFull(xpFullTex);
        xpFull.setScale({xpScaleX, xpScaleY});
        xpFull.setTextureRect(sf::IntRect(
            {0, 0},
            {static_cast<int>(xpFullTex.getSize().x * xpPercent_),
             static_cast<int>(xpFullTex.getSize().y)}));
        xpFull.setPosition({hotbarX, xpBarY});
        window.draw(xpFull);

        // 等级文字（经验条右侧）
        xpLevelText_.setString("Lv." + std::to_string(playerLevel_));
        sf::FloatRect xpLvBounds = xpLevelText_.getLocalBounds();
        xpLevelText_.setPosition({hotbarX + xpBarWidth + 6.0f,
                                  xpBarY + (XP_BAR_HEIGHT - xpLvBounds.size.y) / 2.0f});
        window.draw(xpLevelText_);
    }

    // ========== 6. 绘制物品栏背景 ==========
    hotbarSprite_->setPosition({hotbarX, hotbarY});
    window.draw(*hotbarSprite_);

    // ========== 7. 绘制武器图标 + 右下角等级 ==========
    float slotWidth = hotbarW / 6.0f;
    float slotHeight = hotbarH;
    float targetIconSize = slotHeight * 0.6f;

    for (size_t i = 0; i < weapons_.size() && i < 6; ++i)
    {
        Weapon *weapon = weapons_[i];
        if (!weapon)
            continue;

        const std::string &texId = weapon->getTextureId();
        if (texId.empty() || !ResourceManager::GetInstance().HasTexture(texId))
            continue;

        sf::Texture &tex = ResourceManager::GetInstance().GetTexture(texId);
        sf::Sprite sprite(tex);

        float scale = targetIconSize / std::max(tex.getSize().x, tex.getSize().y);
        sprite.setScale({scale, scale});

        sf::FloatRect bounds = sprite.getLocalBounds();
        float iconW = bounds.size.x * scale;
        float iconH = bounds.size.y * scale;

        float slotCenterX = hotbarX + slotWidth * (static_cast<float>(i) + 0.5f);
        float slotCenterY = hotbarY + slotHeight / 2.0f;

        sprite.setPosition({slotCenterX - iconW / 2.0f, slotCenterY - iconH / 2.0f});
        window.draw(sprite);

        // 右下角等级文字
        sf::Text levelText(font_);
        levelText.setString(std::to_string(weapon->getLevel()));
        levelText.setCharacterSize(14);
        levelText.setFillColor(sf::Color::White);
        levelText.setOutlineColor(sf::Color::Black);
        levelText.setOutlineThickness(1.5f);

        sf::FloatRect ltBounds = levelText.getLocalBounds();
        float lvX = slotCenterX + iconW / 2.0f - ltBounds.size.x - 2.0f + 10.0f;
        float lvY = slotCenterY + iconH / 2.0f - ltBounds.size.y - 2.0f + 4.0f;
        levelText.setPosition({lvX, lvY});
        window.draw(levelText);
    }
}