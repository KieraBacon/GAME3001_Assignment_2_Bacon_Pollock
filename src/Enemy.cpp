#include "Enemy.h"
#include "Game.h"
#include "Util.h"
#include "PlayScene.h"
#include "glm/gtx/string_cast.hpp"

Enemy::Enemy() :
	m_currentDirection(0.0f)
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
	setInitialPts(1.0f);
	newTurn();
}


Enemy::~Enemy()
= default;

void Enemy::draw()
{
	const int xComponent = getPosition().x;
	const int yComponent = getPosition().y;

	TheTextureManager::Instance()->draw("ship", xComponent, yComponent,
		TheGame::Instance()->getRenderer(), m_currentDirection, 255, true);
}

void Enemy::update()
{
}

void Enemy::clean()
{
}
