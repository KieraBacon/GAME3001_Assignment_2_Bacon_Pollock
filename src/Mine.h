#pragma once
#ifndef __Mine__
#define __Mine__


#include "TextureManager.h"
#include "SoundManager.h"
#include "Tile.h"
#include "Scene.h"
#include "PathFindingDisplayObject.h"

class Mine final : public PathFindingDisplayObject
{
public:
     Mine();
	~Mine();

	// Inherited via GameObject
	virtual void draw() override;

	virtual void update() override;

	virtual void clean() override;

private:

};


#endif /* defined (__Mine__) */