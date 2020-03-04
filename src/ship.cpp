#include "ship.h"
#include "Game.h"
#include "Util.h"
#include "PlayScene.h"
#include "glm/gtx/string_cast.hpp"


Ship::Ship() :
	m_maxSpeed(5.0f), m_currentDirection(0.0f), m_turnSpeed(2.0f), m_steerForce(0.1f)
{
	TheTextureManager::Instance()->load("../Assets/textures/ship3.png",
		"ship", TheGame::Instance()->getRenderer());

	auto size = TheTextureManager::Instance()->getTextureSize("ship");
	setWidth(size.x);
	setHeight(size.y);
	setPosition(glm::vec2(400.0f, 300.0f));
	setVelocity(glm::vec2(0.0f, 0.0f));
	setAcceleration(glm::vec2(0.0f, 0.0f));
	setIsColliding(false);
	setType(SHIP);
	setState(IDLE);
	setInitialPts(2.0f);
	newTurn();
}


Ship::~Ship()
= default;

void Ship::draw()
{
	const int xComponent = getPosition().x;
	const int yComponent = getPosition().y;

	TheTextureManager::Instance()->draw("ship", xComponent, yComponent,
		TheGame::Instance()->getRenderer(), m_currentDirection, 255, true);
}

void Ship::m_checkState()
{
	switch (getState())
	{
	case IDLE:
		move();
		break;
	case SEEK:
		m_seek();
		//move();
		break;
	case ARRIVE:
		break;
	case AVOID:
		break;
	case FLEE:
		break;
	default:
		std::cout << "unknown or unused case" << std::endl;
		break;
	}
}

void Ship::update()
{
	move();
}

void Ship::clean()
{
}

void Ship::turnRight()
{
	m_currentDirection += m_turnSpeed;
	if (m_currentDirection >= 360) 
	{
		m_currentDirection = 0;
	}

}

void Ship::turnLeft()
{
	m_currentDirection -= m_turnSpeed;
	if (m_currentDirection < 0)
	{
		m_currentDirection = 359.0;
	}
}

void Ship::move()
{
	if (Util::distance(getPosition(), m_target) > 1.0f) {
		const glm::vec2 desired = Util::normalize(m_target - getPosition()) * m_maxSpeed;
		setVelocity(desired);

		const glm::vec2 newPosition = getPosition() + getVelocity();
		setPosition(newPosition);
	}

}

glm::vec2 Ship::getTarget()
{
	return m_target;
}

void Ship::setTarget(const glm::vec2 position)
{
	m_target = position;
}


void Ship::m_checkBounds()
{

	if (getPosition().x > Config::SCREEN_WIDTH)
	{
		setPosition(glm::vec2(0.0f, getPosition().y));
	}

	if (getPosition().x < 0)
	{
		setPosition(glm::vec2(800.0f, getPosition().y));
	}

	if (getPosition().y > Config::SCREEN_HEIGHT)
	{
		setPosition(glm::vec2(getPosition().x, 0.0f));
	}

	if (getPosition().y < 0)
	{
		setPosition(glm::vec2(getPosition().x, 600.0f));
	}

}

void Ship::m_reset()
{
	setIsColliding(false);
	const int halfWidth = getWidth() * 0.5f;
	const auto xComponent = rand() % (640 - getWidth()) + halfWidth + 1;
	const auto yComponent = -getHeight();
	setPosition(glm::vec2(xComponent, yComponent));
}

void Ship::m_seek()
{
	const auto desired = Util::normalize(m_target - getPosition()) * m_maxSpeed;
	auto steer = (desired - getVelocity());

	steer = Util::limitMagnitude(steer, m_steerForce);
	setAcceleration(steer);
}