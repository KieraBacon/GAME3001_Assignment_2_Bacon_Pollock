#pragma once
#ifndef __TILE__
#define __TILE__

#include <vector>

#include "DisplayObject.h"
#include "TextureManager.h"
#include "Config.h"
#include "Label.h"
#include "Scene.h"

#include "TileState.h"
#include "TileNeighbour.h"
#include "Heuristic.h"

class Tile final : public DisplayObject
{
public:
	enum class Path { NONE, GREEN, RED };
public:
	Tile(glm::vec2 world_position = glm::vec2(), glm::vec2 grid_position = glm::vec2());
	~Tile();

	// Inherited via GameObject
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	void drawFrame(int alpha = 0);
	void drawLabels();
	void drawDots(int alpha = 127);

	// get neighbours
	Tile* getUp();
	Tile* getDown();
	Tile* getRight();
	Tile* getLeft();

	// set neighbours
	void setUp(Tile* tile);
	void setDown(Tile* tile);
	void setRight(Tile* tile);
	void setLeft(Tile* tile);

	glm::vec2 getGridPosition() const;

	void setTileState(TileState state);
	TileState getTileState() const;
	
	float calcTargetDistance(glm::vec2 goal_location);
	void setTargetDistance(float distance);
	float getTargetDistance() const;

	void setTileCost(float cost);
	float getTileCost() const;
	float calcTotalCost(Tile* parent);
	void setTotalCost(float cost);
	float getTotalCost() const;
	
	float getTileValue() const;
	void setTileValue(float new_value);

	void setTileStateLabel(const std::string& closed_open) const;

	void setParentNode(Tile* parent);
	Tile* getParentNode() const;

	void setTilePathColour(Tile::Path colour);
	
	void resetTile();
	
	std::vector<Tile*> getNeighbours() const;

	void setHeuristic(Heuristic heuristic);

	void displayTile();

private:
	float m_tileCost = Config::TILE_COST;
	float m_totalCost = 0.0f;
	float m_targetDist = 0.0f;
	float m_tileValue = 0.0f;
	Tile::Path m_tilePathColour;
	TileState m_tileState;
	glm::vec2 m_gridPosition;

	// labels
	Label* m_pValueLabel;
	Label* m_pHeuristicLabel;
	Label* m_pCostLabel;
	Label* m_pClosedOpenLabel;

	glm::vec2 m_goalLocation;
	Tile* m_pParentNode;
	std::vector<Tile*> m_pNeighbours;
	Heuristic m_heuristic;
};


#endif /* defined (__TILE__) */