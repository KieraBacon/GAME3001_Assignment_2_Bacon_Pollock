#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"
#include "imgui.h"

// Game Objects
#include "Mine.h"
#include "Planet.h"
#include "ship.h"
#include "Tile.h"
#include "Heuristic.h"
#include "Enemy.h"


class PlayScene final : public Scene
{
public:
	PlayScene();
	~PlayScene();

	// Inherited via Scene
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;

	void endTurn();
	const unsigned int getTurnNum();

private:
	// GameObjects
	Ship* m_pShip;
	Planet* m_pPlanet;
	Label* m_pTurnLabel{};
	Label* m_pScoreLabel{};
	Label* m_pPtsLabel{};

	// Tile & Grid members
	std::vector<Tile*> m_pGrid;

	// mines
	int m_mineNum = Config::MINE_NUM;
	std::vector<Mine*> m_mines;
	void m_buildMines();
	void m_eraseMines();
	void m_resetImpassableTiles();
	void m_spawnMines();
	void m_minePassAdjustment();

	// enemies
	int m_enemyNum = Config::ENEMY_NUM;
	std::vector<Enemy*> m_enemies;
	void m_buildEnemies();
	void m_spawnEnemies();
	void m_eraseEnemies();
	void m_markEnemyTiles();
	bool m_lookInCardinalDirections(Enemy* enemy);
	bool m_checkLineOfSight(Tile* startingTile, TileNeighbour direction);

	void m_resetGrid();
	void m_findObjects();
	void m_buildGrid();
	void m_randomizeTileCosts();
	void m_mapTiles();

	int m_spawnObject(PathFindingDisplayObject* object);
	void m_spawnShip();
	void m_spawnPlanet();

	// pathfinding functions & variables
	Tile* m_findLowestValueInOpenList();
	bool m_findShortestPath(PathFindingDisplayObject* actor);
	std::vector<Tile*> m_openList;
	std::vector<Tile*> m_closedList;
	Heuristic m_heuristic;
	void m_selectHeuristic(Heuristic heuristic);
	unsigned int m_turnNum;

	glm::vec2 m_mousePosition;

	// ImGui utility functions
	void m_ImGuiKeyMap();
	void m_ImGuiSetStyle();
	void m_updateUI();

	// ImGui menu variables
	bool m_exitApp = false;
	bool m_displayAbout = false;
	bool m_displayUI = true;

	// ImGui button variables
	ImVec4 m_manhattanButtonColour;
	ImVec4 m_euclideanButtonColour;

	// ImGui visibility variables
	bool m_shipVisible = true;
	bool m_planetVisible = true;
	bool m_minesVisible = true;
	bool m_enemiesVisible = true;
	bool m_labelsVisible = true;
	bool m_tilesVisible = true;
	
	void m_resetAll();

};

#endif /* defined (__PLAY_SCENE__) */