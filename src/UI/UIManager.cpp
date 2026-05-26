#include "UIManager.h"
#include "../Player/player.h"
#include "../Weapon/weapon.h"
#include "../Resourcemanager/ResourceManager.h"

UIManager::UIManager()
    : font_("assets/fonts/minecraft.ttf"), weaponInfoText_(font_)
{
    weaponInfoText_.setCharacterSize(16);
    weaponInfoText_.setFillColor(sf::Color::White);
    weaponInfoText_.setPosition({20, 50});

    if (ResourceManager::GetInstance().HasTexture("hotbar"))
    {
        sf::Texture &tex = ResourceManager::GetInstance().GetTexture("hotbar");
        hotbarSprite_ = std::make_unique<sf::Sprite>(tex);
        slotWidth_ = tex.getSize().x / static_cast<float>(HOTBAR_SLOTS);
        slotHeight_ = static_cast<float>(tex.getSize().y);
    }
}

void UIManager::update(const Player &player)
{
    int health = player.getHealth();
    fullHearts_ = health / 2;
    halfHearts_ = (health % 2) == 1 ? 1 : 0;
    emptyHearts_ = MAX_HEARTS - fullHearts_ - halfHearts_;

    weapons_.clear();
    for (const auto &w : player.getWeaponManager().getWeapons())
    {
        weapons_.push_back(w.get());
    }

    if (!weapons_.empty())
    {
        Weapon *w = weapons_[0];
        weaponInfoText_.setString(
            w->getName() + "  Lv." + std::to_string(w->getLevel()) +
            "  DMG:" + std::to_string(w->getDamage()));
    }
    else
    {
        weaponInfoText_.setString("");
    }
}

/*
void UIManager::render(sf::RenderWindow &window)
{
    sf::Vector2f screenSize(static_cast<float>(window.getSize().x),
                            static_cast<float>(window.getSize().y));

    static constexpr float HEART_SCALE = 2.2f;
    static constexpr float HOTBAR_SCALE = 2.2f;
    static constexpr float HEART_GAP = 2.0f;
    static constexpr float HOTBAR_BOTTOM_MARGIN = 10.0f;

    if (!hotbarSprite_)
        return;

    // ---- 计算红心尺寸 ----
    sf::Vector2f heartSize(14.0f, 14.0f); // 默认
    if (ResourceManager::GetInstance().HasTexture("heart_full"))
    {
        auto &ht = ResourceManager::GetInstance().GetTexture("heart_full");
        heartSize = {ht.getSize().x * HEART_SCALE, ht.getSize().y * HEART_SCALE};
    }
    float heartSpacing = heartSize.x + HEART_GAP;

    // ---- 计算物品栏位置 ----
    hotbarSprite_->setScale({HOTBAR_SCALE, HOTBAR_SCALE});
    sf::FloatRect hb = hotbarSprite_->getLocalBounds();
    float hotbarW = hb.size.x * HOTBAR_SCALE;
    float hotbarH = hb.size.y * HOTBAR_SCALE;
    float hotbarX = (screenSize.x - hotbarW) / 2.0f;
    float hotbarY = screenSize.y - hotbarH - HOTBAR_BOTTOM_MARGIN;

    // ---- 武器信息文字（红心上方）----
    weaponInfoText_.setPosition({hotbarX, hotbarY - heartSize.y - 26});

    // ---- 红心血条（物品栏左上角对齐）----
    float heartX = hotbarX;
    float heartY = hotbarY - heartSize.y - 4.0f;

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

    // ---- 物品栏 ----
    hotbarSprite_->setPosition({hotbarX, hotbarY});
    window.draw(*hotbarSprite_);

    // ---- 武器信息文字 ----
    window.draw(weaponInfoText_);

    // ---- 武器贴图 ----
    float scaledSlotW = slotWidth_ * HOTBAR_SCALE;
    float scaledSlotH = slotHeight_ * HOTBAR_SCALE;
    float weaponSize = scaledSlotH * 0.75f;

    for (size_t i = 0; i < weapons_.size() && i < static_cast<size_t>(HOTBAR_SLOTS); ++i)
    {
        Weapon *weapon = weapons_[i];
        if (!weapon)
            continue;

        const std::string &texId = weapon->getTextureId();
        if (texId.empty() || !ResourceManager::GetInstance().HasTexture(texId))
            continue;

        sf::Texture &tex = ResourceManager::GetInstance().GetTexture(texId);
        sf::Sprite sprite(tex);

        float scaleX = weaponSize / tex.getSize().x;
        float scaleY = weaponSize / tex.getSize().y;
        float scale = scaleX < scaleY ? scaleX : scaleY;
        sprite.setScale({scale, scale});

        sf::FloatRect bounds = sprite.getGlobalBounds();
        float slotCenterX = hotbarX + scaledSlotW * (static_cast<float>(i) + 0.5f);
        float slotCenterY = hotbarY + scaledSlotH / 2.0f;
        sprite.setPosition({slotCenterX - bounds.size.x / 2.0f,
                            slotCenterY - bounds.size.y / 2.0f});

        window.draw(sprite);
    }
}
    */
void UIManager::render(sf::RenderWindow &window)
{
    sf::Vector2f screenSize(static_cast<float>(window.getSize().x),
                            static_cast<float>(window.getSize().y));

    static constexpr float HEART_SCALE = 2.2f;
    static constexpr float HOTBAR_SCALE = 2.2f;
    static constexpr float HEART_GAP = 2.0f;
    static constexpr float HOTBAR_BOTTOM_MARGIN = 10.0f;

    if (!hotbarSprite_)
        return;

    // ========== 1. 计算物品栏位置 ==========
    hotbarSprite_->setScale({HOTBAR_SCALE, HOTBAR_SCALE});
    sf::FloatRect hb = hotbarSprite_->getLocalBounds();
    float hotbarW = hb.size.x * HOTBAR_SCALE; // 120 * 2.2 = 264
    float hotbarH = hb.size.y * HOTBAR_SCALE; // 20 * 2.2 = 44
    float hotbarX = (screenSize.x - hotbarW) / 2.0f;
    float hotbarY = screenSize.y - hotbarH - HOTBAR_BOTTOM_MARGIN;

    // ========== 2. 计算红心尺寸 ==========
    sf::Vector2f heartSize(14.0f, 14.0f);
    if (ResourceManager::GetInstance().HasTexture("heart_full"))
    {
        auto &ht = ResourceManager::GetInstance().GetTexture("heart_full");
        heartSize = {ht.getSize().x * HEART_SCALE, ht.getSize().y * HEART_SCALE};
    }
    float heartSpacing = heartSize.x + HEART_GAP;

    // ========== 3. 武器信息文字 ==========
    weaponInfoText_.setPosition({hotbarX, hotbarY - heartSize.y - 26});

    // ========== 4. 红心血条 ==========
    float heartX = hotbarX;
    float heartY = hotbarY - heartSize.y - 4.0f;

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

    // ========== 5. 绘制物品栏背景 ==========
    hotbarSprite_->setPosition({hotbarX, hotbarY});
    window.draw(*hotbarSprite_);

    // ========== 6. 绘制武器信息文字 ==========
    window.draw(weaponInfoText_);

    // ========== 7. 绘制武器图标 ==========
    // 每个槽位宽度 = 264 / 6 = 44
    float slotWidth = hotbarW / 6.0f;
    float slotHeight = hotbarH;

    // 武器图标目标大小（槽位的 60%）
    float targetIconSize = slotHeight * 0.6f; // 44 * 0.6 = 26.4

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

        // 计算缩放（保持宽高比，目标大小 targetIconSize）
        float scale = targetIconSize / std::max(tex.getSize().x, tex.getSize().y);
        sprite.setScale({scale, scale});

        // 获取缩放后的实际尺寸
        sf::FloatRect bounds = sprite.getLocalBounds();
        float iconW = bounds.size.x * scale;
        float iconH = bounds.size.y * scale;

        // 槽位中心位置
        float slotCenterX = hotbarX + slotWidth * (static_cast<float>(i) + 0.5f);
        float slotCenterY = hotbarY + slotHeight / 2.0f;

        // 图标居中
        sprite.setPosition({slotCenterX - iconW / 2.0f, slotCenterY - iconH / 2.0f});

        window.draw(sprite);
    }
}