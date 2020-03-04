#pragma once
#ifndef __PLANET__
#define __PLANET__

#include "PathFindingDisplayObject.h"


class Planet final : public PathFindingDisplayObject
{
public:
	Planet();
	~Planet();

	// Inherited via GameObject
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;

private:
};


#endif /* defined (__PLANET__) */