#include "Mine.h"
#include "Game.h"

Mine::Mine()
{
	TheTextureManager::Instance()->load("../Assets/textures/mine.png",
		"mine", TheGame::Instance()->getRenderer());

	const auto size = TheTextureManager::Instance()->getTextureSize("mine");
	setWidth(size.x);
	setHeight(size.y);
	setPosition(glm::vec2(0.0f, 0.0f));
	setVelocity(glm::vec2(0.0f, 0.0f));
	setType(MINE);
}

Mine::~Mine()
= default;

void Mine::draw()
{
	const int xComponent = getPosition().x;
	const int yComponent = getPosition().y;

	TheTextureManager::Instance()->draw("mine", xComponent, yComponent,
		TheGame::Instance()->getRenderer(), 0, 255, true);
}

void Mine::update()
{
}

void Mine::clean()
{
}
