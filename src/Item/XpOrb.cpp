#include "XpOrb.h"
#include "../Resourcemanager/ResourceManager.h"
#include <cmath>

XpOrb::XpOrb(sf::Vector2f pos, int xpValue)
	: DropItem(pos, xpValue, 120.0f),
	  idleTimer_(1.0f), attractRange_(150.0f),
	  minSpeed_(100.0f), maxSpeed_(400.0f),
	  isMoving_(false),
	  frameCol_(0), frameRow_(0), animTimer_(0.0f)
{
	shape_.setSize({32.0f, 32.0f});
	shape_.setOrigin({16.0f, 16.0f});
	shape_.setFillColor(sf::Color(100, 255, 100));

	// 加载 64x64 贴图（4x4 网格，每帧 16x16）
	if (ResourceManager::GetInstance().HasTexture("experience_orb"))
	{
		sf::Texture &tex = ResourceManager::GetInstance().GetTexture("experience_orb");
		shape_.setTexture(&tex);
		shape_.setTextureRect(sf::IntRect({0, 0}, {FRAME_SIZE, FRAME_SIZE}));
	}
}

void XpOrb::update(float deltaTime, const sf::Vector2f &playerPos)
{
	if (!alive_)
		return;

	elapsed_ += deltaTime;
	if (elapsed_ >= lifetime_)
	{
		alive_ = false;
		return;
	}

	// 动画：按行循环，1秒16帧
	animTimer_ += deltaTime;
	if (animTimer_ >= ANIM_CYCLE)
		animTimer_ -= ANIM_CYCLE;

	int frameIdx = static_cast<int>(animTimer_ / FRAME_TIME) % (GRID_SIZE * GRID_SIZE);
	frameCol_ = frameIdx % GRID_SIZE;
	frameRow_ = frameIdx / GRID_SIZE;
	shape_.setTextureRect(sf::IntRect(
		{frameCol_ * FRAME_SIZE, frameRow_ * FRAME_SIZE},
		{FRAME_SIZE, FRAME_SIZE}));

	// 静止1秒后才开始追踪
	if (!isMoving_)
	{
		idleTimer_ -= deltaTime;
		if (idleTimer_ <= 0.0f)
			isMoving_ = true;
	}

	// 追踪玩家
	if (isMoving_)
	{
		sf::Vector2f toPlayer = playerPos - position_;
		float dist = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

		if (dist < attractRange_ && dist > 0.01f)
		{
			toPlayer /= dist;

			// 距离越近速度越快：150px时100，0px时400
			float speed = maxSpeed_ - (dist / attractRange_) * (maxSpeed_ - minSpeed_);
			if (speed < minSpeed_)
				speed = minSpeed_;
			if (speed > maxSpeed_)
				speed = maxSpeed_;

			position_ += toPlayer * speed * deltaTime;
		}
	}

	shape_.setPosition(position_);
}

void XpOrb::render(sf::RenderWindow &window)
{
	if (!alive_)
		return;
	shape_.setPosition(position_);
	window.draw(shape_);
}