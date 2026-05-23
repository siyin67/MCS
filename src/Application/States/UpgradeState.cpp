#include "UpgradeState.h"
#include "../StateManager.h"
#include "Resourcemanager/ResourceManager.h"
#include <iostream>

UpgradeState::UpgradeState(Player *player)
    : player_(player), selectedIndex_(0), font_("assets/fonts/minecraft.ttf"), titleText_(font_)
{
    std::cout << "UpgradeState created" << std::endl;

    bg_.setFillColor(sf::Color(0, 0, 0, 180));
    bg_.setSize({1280, 720});

    // 加载升级界面背景（包含三个按钮）
    if (ResourceManager::GetInstance().HasTexture("upgrade_background"))
    {
        panelBg_.setTexture(&ResourceManager::GetInstance().GetTexture("upgrade_background"));
        std::cout << "Loaded upgrade_background texture" << std::endl;
    }
    panelBg_.setFillColor(sf::Color::White);

    // 加载武器图标框纹理
    if (ResourceManager::GetInstance().HasTexture("upgradeitem_background"))
    {
        iconFrame_.setTexture(&ResourceManager::GetInstance().GetTexture("upgradeitem_background"));
    }
    iconFrame_.setFillColor(sf::Color::White);

    // 加载武器图标
    if (ResourceManager::GetInstance().HasTexture("iron_sword"))
    {
        icon_.setTexture(&ResourceManager::GetInstance().GetTexture("iron_sword"));
        sf::FloatRect bounds = icon_.getLocalBounds();
        icon_.setOrigin({bounds.size.x / 2, bounds.size.y / 2});
    }
    icon_.setFillColor(sf::Color::White);

    titleText_.setString("LEVEL UP!");
    titleText_.setCharacterSize(64);
    titleText_.setFillColor(sf::Color::White);
}

void UpgradeState::handleInput(sf::RenderWindow &window)
{
    static bool wasPressed = false;

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    float mouseX = static_cast<float>(mousePos.x);
    float mouseY = static_cast<float>(mousePos.y);

    sf::Vector2u windowSize = window.getSize();
    float centerX = windowSize.x / 2.0f;
    float buttonWidth = 250;
    float buttonHeight = 200;
    float spacing = 30;
    float startX = centerX - (buttonWidth * 3 + spacing * 2) / 2;
    float buttonY = windowSize.y / 2.0f - buttonHeight / 2;

    for (int i = 0; i < 3; i++)
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
    std::cout << "Option " << index << " selected" << std::endl;
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

    // 绘制升级界面背景（包含三个按钮）
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

    for (int i = 0; i < 3; i++)
    {
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
        window.draw(icon_);

        // 武器名称
        sf::Text nameText(font_);
        nameText.setString("Iron Sword");
        nameText.setCharacterSize(22);
        nameText.setFillColor(sf::Color::White);
        sf::FloatRect nameBounds = nameText.getLocalBounds();
        nameText.setPosition({buttonX + buttonWidth / 2 - nameBounds.size.x / 2, buttonY + 145});
        window.draw(nameText);

        // 武器描述
        sf::Text descText(font_);
        descText.setString("Swing in a wide arc");
        descText.setCharacterSize(16);
        descText.setFillColor(sf::Color(200, 200, 200));
        descText.setPosition({buttonX + 15, buttonY + 195});
        window.draw(descText);

        // 属性文字
        sf::Text attrText(font_);
        attrText.setString("Dmg:20  CD:0.8s");
        attrText.setCharacterSize(16);
        attrText.setFillColor(sf::Color(150, 150, 150));
        attrText.setPosition({buttonX + 15, buttonY + 240});
        window.draw(attrText);

        // 边框高亮
        sf::RectangleShape outline;
        outline.setSize({buttonWidth, buttonHeight});
        outline.setPosition({buttonX, buttonY});
        outline.setFillColor(sf::Color::Transparent);
        outline.setOutlineThickness(i == selectedIndex_ ? 4 : 2);
        outline.setOutlineColor(i == selectedIndex_ ? sf::Color::Yellow : sf::Color::White);
        window.draw(outline);
    }
}