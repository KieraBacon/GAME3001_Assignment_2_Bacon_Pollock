#pragma once
#include "ship.h"

class Enemy final : public PathFindingDisplayObject
{
public: enum class EnemyState { IDLE, CHASING };
public:
	Enemy();
	~Enemy();

	// Inherited via GameObject
	void draw() override;
	void update() override;
	void clean() override;

	void setState(EnemyState state) { m_state = state; }
	EnemyState getState() { return m_state; }
private:
	double m_currentDirection;
	EnemyState m_state;
};