#pragma once
#include "DropItem.h"

class XpOrb : public DropItem
{
private:
	float idleTimer_;      // 生成后静止计时（1秒）
	float attractRange_;   // 吸引范围（150像素）
	float minSpeed_;       // 最远吸引速度
	float maxSpeed_;       // 最近吸引速度
	bool isMoving_;        // 是否开始追踪玩家

	// 动画：4x4 网格，16帧，1秒周期
	int frameCol_;
	int frameRow_;
	float animTimer_;
	static constexpr int GRID_SIZE = 4;
	static constexpr int FRAME_SIZE = 16;
	static constexpr float ANIM_CYCLE = 1.0f;   // 1秒16帧
	static constexpr float FRAME_TIME = 1.0f / 16.0f;

public:
	XpOrb(sf::Vector2f pos, int xpValue);

	void update(float deltaTime, const sf::Vector2f &playerPos) override;
	void render(sf::RenderWindow &window) override;
};