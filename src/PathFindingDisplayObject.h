#pragma once
#ifndef  __PATH_FINDING_DISPLAY_OBJECT__
#define __PATH_FINDING_DISPLAY_OBJECT__

#include "DisplayObject.h"

class PathFindingDisplayObject : public DisplayObject
{
public:
	friend class Tile;
	PathFindingDisplayObject();
	virtual ~PathFindingDisplayObject();
	virtual void draw() override = 0;
	virtual void update() override = 0;
	virtual void clean() override = 0;

	// pathfinding behaviours
	Tile* getTile() const;
	void setTile(Tile* new_tile);

	// turn-based behaviours
	void newTurn();
	
	void setInitialPts(float pts);
	float getInitialPtr();
	void setCurrentPts(float pts);
	float getCurrentPts();

	void addPathNodeToBack(Tile* node);
	void moveAlongPath();
	std::vector<Tile*>& getPath();
	void clearPath();

	Tile* getTargetTile();
	void setTargetTile(Tile* target);
private:
	Tile* m_currentTile;

	// turn-based behaviours
	bool m_usePts(float amount);
	float m_initialPts;
	float m_currentPts;
	std::vector<Tile*> m_path;
	Tile* m_targetTile;
};

#endif /* defined (__PATH_FINDING_DISPLAY_OBJECT__)*/
