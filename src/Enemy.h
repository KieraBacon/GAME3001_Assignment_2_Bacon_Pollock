#pragma once
#include "ship.h"

class Enemy final : public PathFindingDisplayObject
{
public:
	Enemy();
	~Enemy();

	// Inherited via GameObject
	void draw() override;
	void update() override;
	void clean() override;

private:
	double m_currentDirection;
};