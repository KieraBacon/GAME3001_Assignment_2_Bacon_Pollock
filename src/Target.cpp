#include "Target.h"
#include "Game.h"

Target::Target()
{
	TheTextureManager::Instance()->load("../Assets/textures/Circle.png",
		"circle", TheGame::Instance()->getRenderer());

	const auto size = TheTextureManager::Instance()->getTextureSize("circle");
	setWidth(size.x);
	setHeight(size.y);
	setPosition(glm::vec2(100.0f, 100.0f));
	setVelocity(glm::vec2(0, 0));
	setIsColliding(false);
	setType(TARGET);
}

Target::~Target()
= default;

void Target::draw()
{
	const int xComponent = getPosition().x;
	const int yComponent = getPosition().y;
	TheTextureManager::Instance()->draw("circle", xComponent, yComponent,
		TheGame::Instance()->getRenderer(), true);
}

void Target::update()
{
	m_move();
	m_checkBounds();
}

void Target::clean()
{
}

void Target::m_move()
{
	const auto newPosition = getPosition() + getVelocity() * 5.0f;
	setPosition(newPosition);
}

void Target::m_checkBounds()
{
}

void Target::m_reset()
{
}
