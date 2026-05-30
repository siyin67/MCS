#include "MenuState.h"
#include <iostream>
#include <fstream>
#include "Resourcemanager/ResourceManager.h"
#include "GameState.h"
#include "../Statemanager.h"
#include "Sound/SoundManager.h"
#include "../../Net/MojangAPI.h"
#include "../../Player/player.h"

MenuState::MenuState() : font_("assets/fonts/minecraft.ttf"), text_start_(font_), text_exit_(font_),
    input_text_(font_), headLoaded_(false)
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
    if (ResourceManager::GetInstance().HasTexture("text_input"))
    {
        sf::Texture &bgTex = ResourceManager::GetInstance().GetTexture("text_input");
        input_bg_.setTexture(&bgTex);
    }
    if (ResourceManager::GetInstance().HasTexture("text_record"))
    {
        sf::Texture &bgTex = ResourceManager::GetInstance().GetTexture("text_record");
        confirm_button_.setTexture(&bgTex);
    }
    if (ResourceManager::GetInstance().HasTexture("menu_bg"))
    {
        sf::Texture &bgTex = ResourceManager::GetInstance().GetTexture("menu_bg");
        background_.setTexture(&bgTex);
    }
    else
    {
        background_.setFillColor(sf::Color(50, 50, 100));
    }

    // 头像预览：优先使用已缓存的自定义皮肤
    if (!Player::customSkinPath.empty() && ResourceManager::GetInstance().HasTexture("player_skin"))
    {
        head_preview_.setTexture(&ResourceManager::GetInstance().GetTexture("player_skin"), true);
    }
    else if (ResourceManager::GetInstance().HasTexture("player"))
    {
        head_preview_.setTexture(&ResourceManager::GetInstance().GetTexture("player"), true);
    }
    head_preview_.setSize({48, 48});

    text_start_.setString("Start Game");
    text_exit_.setString("Exit Game");
    input_text_.setString("");

    start_button_.setSize({400, 50});
    exit_button_.setSize({400, 50});
    input_bg_.setSize({320, 40});
    confirm_button_.setSize({60, 40});

    title_.setSize({700, 250});

    text_start_.setCharacterSize(20);
    text_exit_.setCharacterSize(20);
    input_text_.setCharacterSize(18);

    text_start_.setFillColor(sf::Color::White);
    text_exit_.setFillColor(sf::Color::White);
    input_text_.setFillColor(sf::Color::White);

    // 首页bgm播放
    std::vector<std::string> playlist = {
        "assets/bgm/13 Wet Hands.mp3",
        "assets/bgm/18 Sweden.mp3",
        "assets/bgm/03 Subwoofer Lullaby.mp3"};
    SoundManager::getInstance().setBGMPlaylist(playlist);
    SoundManager::getInstance().playRandomBGM();
    SoundManager::getInstance().setBGMVolume(20);
}

MenuState::~MenuState()
{
    if (headThread_.joinable())
        headThread_.detach();
}

void MenuState::updateLayout(sf::Vector2u winSize)
{
    float cx = winSize.x / 2.0f;
    float cy = winSize.y / 2.0f;

    background_.setSize(sf::Vector2f(winSize.x, winSize.y));
    background_.setPosition({0, 0});

    start_button_.setPosition({cx - 200, cy + 50});
    exit_button_.setPosition({cx - 200, cy + 120});
    input_bg_.setPosition({cx - 200, cy + 190});
    confirm_button_.setPosition({cx + 130, cy + 190});
    head_preview_.setPosition({cx + 200, cy + 190});

    title_.setPosition({cx - 350, winSize.y * 0.20f - 70});

    text_start_.setPosition({cx - text_start_.getLocalBounds().size.x / 2,
                              cy + 50 + 25 - text_start_.getLocalBounds().size.y / 2});
    text_exit_.setPosition({cx - text_exit_.getLocalBounds().size.x / 2,
                            cy + 120 + 25 - text_exit_.getLocalBounds().size.y / 2});
    input_text_.setPosition({cx - 190, cy + 190 + 20 - input_text_.getLocalBounds().size.y / 2});
}

void MenuState::tryLoadHead()
{
    if (playerName_.empty() || headLoaded_ || headLoading_.load())
        return;

    headLoading_ = true;
    headReady_ = false;
    std::string name = playerName_;

    if (headThread_.joinable())
        headThread_.detach();

    headThread_ = std::thread([this, name]() {
        std::string path = "assets/images/skin/" + name + ".png";
        bool ok = false;
        // 优先从本地缓存加载
        std::ifstream testFile(path, std::ios::binary);
        if (testFile.good())
        {
            testFile.close();
            std::cout << "[Menu] Using cached skin: " << path << std::endl;
            ok = true;
        }
        else
        {
            std::cout << "[Menu] Fetching from Mojang API..." << std::endl;
            ok = MojangAPI::fetchPlayerHead(name, path);
        }
        headResultPath_ = path;
        headResultOk_ = ok;
        headReady_ = true;
    });
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
            std::cout << "Start Game, Player: " << playerName_ << std::endl;
            stateManager_->changeState(std::make_unique<GameState>());
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

    if (confirm_button_.getGlobalBounds().contains(mouseCoords))
    {
        confirm_button_.setOutlineThickness(2);
        confirm_button_.setOutlineColor(sf::Color::White);

        if (leftPressed && !wasPressed)
        {
            std::cout << "Fetching head for: " << playerName_ << std::endl;
            tryLoadHead();
        }
    }
    else
    {
        confirm_button_.setOutlineThickness(0);
    }

    wasPressed = leftPressed;
}

void MenuState::handleEvent(const sf::Event &event)
{
    if (const auto *textEvent = event.getIf<sf::Event::TextEntered>())
    {
        auto ch = textEvent->unicode;
        if (ch == '\b')
        {
            if (!playerName_.empty())
                playerName_.pop_back();
        }
        else if (ch >= 32 && ch < 127)
        {
            if (playerName_.size() < 16)
                playerName_ += static_cast<char>(ch);
        }
        input_text_.setString(playerName_);
        headLoaded_ = false; // 名字变了，允许重新加载
    }
}

void MenuState::update(float deltaTime)
{
    (void)deltaTime;

    if (headReady_.load())
    {
        headReady_ = false;
        headLoading_ = false;

        if (headThread_.joinable())
            headThread_.detach();

        if (headResultOk_)
        {
            ResourceManager::GetInstance().LoadTexture("player_skin", headResultPath_);
            if (ResourceManager::GetInstance().HasTexture("player_skin"))
            {
                auto &tex = ResourceManager::GetInstance().GetTexture("player_skin");
                tex.setSmooth(false);
                head_preview_.setTexture(&tex, true);
            }
            Player::customSkinPath = headResultPath_;
        }
        headLoaded_ = true;
    }
}

void MenuState::render(sf::RenderWindow &window)
{
    sf::Vector2u winSize = window.getSize();
    window.setView(sf::View(sf::Vector2f(winSize.x / 2.f, winSize.y / 2.f), sf::Vector2f(winSize)));
    updateLayout(winSize);

    window.draw(background_);
    window.draw(start_button_);
    window.draw(exit_button_);
    window.draw(input_bg_);
    window.draw(confirm_button_);
    window.draw(head_preview_);
    window.draw(title_);
    window.draw(text_start_);
    window.draw(text_exit_);
    window.draw(input_text_);
}
