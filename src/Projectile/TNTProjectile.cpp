#include "TNTProjectile.h"
#include "Monster/Monster.h"
#include "Resourcemanager/ResourceManager.h"
#include "Sound/SoundManager.h"
#include <cmath>
#include <iostream>

TNTProjectile::TNTProjectile(sf::Vector2f startPos, sf::Vector2f targetPos,
							 int damage, float radius)
	: state_(TNTState::FLYING), targetPos_(targetPos),
	  speed_(500.0f), explosionRadius_(radius),
	  flashOn_(true), flashTimer_(0.0f),
	  explosionFrame_(0), explosionFrameTimer_(0.0f),
	  damageDealt_(false)
{
	damage_ = damage;
	isActive_ = true;

	// 飞行态使用 tnt_button 贴图
	setTextureId("tnt_button");
	setSize({24, 24});
	shape_.setFillColor(sf::Color::White);

	// 计算飞行方向
	sf::Vector2f dir = targetPos_ - startPos;
	float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
	if (dist > 0.01f)
	{
		dir.x /= dist;
		dir.y /= dist;
	}
	velocity_ = dir * speed_;
	position_ = startPos;
	shape_.setPosition(position_);

	// 加载爆炸动画帧（16帧）
	bool allLoaded = true;
	for (int i = 0; i < EXPLOSION_FRAMES; i++)
	{
		std::string path = "assets/images/effects/tnt/explosion_" +
						   std::to_string(i) + ".png";
		if (!explosionTex_[i].loadFromFile(path))
		{
			allLoaded = false;
			std::cerr << "Failed to load: " << path << std::endl;
		}
	}
	if (!allLoaded)
	{
		std::cerr << "Some explosion textures missing, using fallback color" << std::endl;
	}
}

void TNTProjectile::update(float deltaTime)
{
	if (!isActive_)
		return;

	switch (state_)
	{
	case TNTState::FLYING:
	{
		sf::Vector2f toTarget = targetPos_ - position_;
		float dist = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
		if (dist < 8.0f)
		{
			position_ = targetPos_;
			velocity_ = {0, 0};
			state_ = TNTState::LANDED;
			stateTimer_ = LANDED_DURATION;
			flashTimer_ = 0.0f;
			flashOn_ = true;
		}
		else
		{
			toTarget.x /= dist;
			toTarget.y /= dist;
			position_ += toTarget * speed_ * deltaTime;
		}
		shape_.setPosition(position_);
		break;
	}

	case TNTState::LANDED:
	{
		stateTimer_ -= deltaTime;
		// 闪烁
		flashTimer_ += deltaTime;
		if (flashTimer_ >= 0.08f)
		{
			flashTimer_ = 0.0f;
			flashOn_ = !flashOn_;
			shape_.setFillColor(flashOn_ ? sf::Color::White : sf::Color(255, 60, 60));
		}
		if (stateTimer_ <= 0.0f)
		{
			state_ = TNTState::EXPLODING;
			explosionFrame_ = -1;
			explosionFrameTimer_ = 0.0f;
			damageDealt_ = false;

			// 切换到爆炸贴图
			if (explosionTex_[0].getSize().x > 0)
			{
				auto sz = explosionTex_[0].getSize();
				shape_.setSize({(float)sz.x, (float)sz.y});
				shape_.setOrigin({(float)sz.x / 2, (float)sz.y / 2});
			}
			else
			{
				// 无贴图时用红色圆形示意
				float dia = explosionRadius_ * 2;
				shape_.setSize({dia, dia});
				shape_.setOrigin({dia / 2, dia / 2});
				shape_.setFillColor(sf::Color(255, 100, 0, 200));
			}
			SoundManager::getInstance().playSFX("tnt_explode");
		}
		break;
	}

	case TNTState::EXPLODING:
	{
		explosionFrameTimer_ += deltaTime;
		float frameTime = EXPLOSION_DURATION / EXPLOSION_FRAMES;
		int frame = static_cast<int>(explosionFrameTimer_ / frameTime);
		if (frame >= EXPLOSION_FRAMES)
		{
			state_ = TNTState::DEAD;
			isActive_ = false;
			break;
		}
		if (frame != explosionFrame_)
		{
			explosionFrame_ = frame;
			if (explosionTex_[frame].getSize().x > 0)
			{
				shape_.setTexture(&explosionTex_[frame]);
				auto sz = explosionTex_[frame].getSize();
				shape_.setSize({(float)sz.x, (float)sz.y});
				shape_.setOrigin({(float)sz.x / 2, (float)sz.y / 2});
			}
		}
		shape_.setPosition(position_);
		break;
	}

	case TNTState::DEAD:
		isActive_ = false;
		break;
	}
}

void TNTProjectile::render(sf::RenderWindow &window)
{
	if (!isActive_)
		return;
	window.draw(shape_);
}

void TNTProjectile::onHit(Monster &monster)
{
	(void)monster;
	// TNT 不通过直接碰撞造成伤害，由 checkAreaDamage 处理爆炸
}

void TNTProjectile::checkAreaDamage(std::vector<std::unique_ptr<Monster>> &monsters)
{
	if (state_ != TNTState::EXPLODING)
		return;
	if (damageDealt_)
		return;
	damageDealt_ = true;

	for (auto &m : monsters)
	{
		if (!m->isAlive())
			continue;

		sf::Vector2f delta = m->getPosition() - position_;
		float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
		if (dist <= explosionRadius_)
		{
			// 距离越远伤害越低（边缘50%）
			float dmgFactor = 1.0f - (dist / explosionRadius_) * 0.5f;
			int actualDmg = static_cast<int>(damage_ * dmgFactor);
			m->takeDamage(actualDmg);
		}
	}
}

sf::FloatRect TNTProjectile::getGlobalBounds() const
{
	// 飞行/落地时不参与标准碰撞
	// 爆炸时由 checkAreaDamage 处理圆形判定
	return sf::FloatRect({0, 0}, {0, 0});
}