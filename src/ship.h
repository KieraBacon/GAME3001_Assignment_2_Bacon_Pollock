#pragma once
#ifndef __SHIP__
#define __SHIP__

#include "PathFindingDisplayObject.h"
#include "TextureManager.h"

class Ship final : public PathFindingDisplayObject
{
public:
	Ship();
	~Ship();

	// Inherited via GameObject
	void draw() override;
	void update() override;
	void clean() override;

	void turnRight();
	void turnLeft();
	void move();

	glm::vec2 getTarget();
	void setTarget(glm::vec2 position);

private:


	void m_checkState();
	void m_checkBounds();

	void m_reset();

	// steering behaviours
	void m_seek();
	float m_maxSpeed;
	double m_currentDirection;
	float m_turnSpeed;
	float m_steerForce;
	glm::vec2 m_target;
};


#endif /* defined (__SHIP__) */

