#pragma once
#pragma once
#ifndef __DEFEAT_SCENE__
#define __DEFEAT_SCENE__

#include "Scene.h"
#include "Label.h"

class DefeatScene final : public Scene
{
public:
	DefeatScene();
	~DefeatScene();

	// Inherited via Scene
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;

private:
	Label* m_pHeaderLabel{};
	Label* m_pInstructionsLabel{};
	Label* m_pScoreLabel{};
};

#endif /* defined (__DEFEAT_SCENE__) */