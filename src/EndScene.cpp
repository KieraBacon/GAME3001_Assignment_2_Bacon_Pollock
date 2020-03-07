#include "EndScene.h"
#include <algorithm>
#include "Game.h"
#include "glm/gtx/string_cast.hpp"
#include <iomanip>
#include <sstream>

EndScene::EndScene()
{
	EndScene::start();
}

EndScene::~EndScene()
= default;

void EndScene::draw()
{
	m_pHeaderLabel->draw();
	m_pInstructionsLabel->draw();
	m_pScoreLabel->draw();
}

void EndScene::update()
{
}

void EndScene::clean()
{
	std::cout << "Clean called on EndScene" << std::endl;

	delete m_pHeaderLabel;
	m_pHeaderLabel = nullptr;

	delete m_pInstructionsLabel;
	m_pInstructionsLabel = nullptr;

	delete m_pScoreLabel;
	m_pScoreLabel = nullptr;
	removeAllChildren();
}

void EndScene::handleEvents()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			TheGame::Instance()->quit();
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				TheGame::Instance()->quit();
				break;
			case SDLK_1:
				TheGame::Instance()->changeSceneState(SceneState::PLAY_SCENE);
				break;
			case SDLK_2:
				TheGame::Instance()->changeSceneState(SceneState::START_SCENE);
				break;
			}
			break;
		default:
			break;
		}
	}
}

void EndScene::start()
{
	SDL_SetRenderDrawColor(Game::Instance()->getRenderer(), 255, 255, 200, 255);

	const SDL_Color blue = { 0, 0, 255, 255 };
	m_pHeaderLabel = new Label("GOAL REACHED", "BASKVILL", 80, blue, glm::vec2(400.0f, 40.0f));
	m_pHeaderLabel->setParent(this);
	addChild(m_pHeaderLabel);

	m_pInstructionsLabel = new Label("Press 1 to Play Again", "BASKVILL", 40, blue, glm::vec2(400.0f, 120.0f));
	m_pInstructionsLabel->setParent(this);
	addChild(m_pInstructionsLabel);

	std::ostringstream scoreString;
	scoreString << "Score: " << std::fixed << std::setprecision(0) << Game::Instance()->getScore();
	m_pScoreLabel = new Label(scoreString.str(), "BASKVILL", 40, blue, glm::vec2(400.0f, 80.0f));
	m_pScoreLabel->setParent(this);
	addChild(m_pScoreLabel);
}
