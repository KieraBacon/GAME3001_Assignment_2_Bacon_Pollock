#include "PlayScene.h"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include "Game.h"
#include "glm/gtx/string_cast.hpp"
#include "SceneState.h"
#include "Util.h"
#include "IMGUI_SDL/imgui_sdl.h"
#include "TileComparators.h"
#include <sstream>


// Pathfinding & Steering functions ***********************************************

void PlayScene::m_buildMines()
{
	for (int index = 0; index < m_mineNum; ++index)
	{
		auto mine = new Mine();
		addChild(mine);
		m_mines.push_back(mine);
	}
}

void PlayScene::m_eraseMines()
{
	for (auto mine : m_mines)
	{
		delete mine;
		mine = nullptr;
	}
	m_mines.clear();
	m_mines.resize(0);
	m_mines.shrink_to_fit();
}

void PlayScene::m_resetImpassableTiles()
{
	for (auto tile : m_pGrid)
	{
		if(tile->getTileState() == IMPASSABLE)
		{
			tile->setTileState(UNVISITED);
		}
	}
}

void PlayScene::m_spawnMines()
{
	bool navigable = false;
	int reductionNum = 1;
	while (!navigable)
	{
		m_resetGrid();
		m_resetImpassableTiles();
		m_pShip->getTile()->setTileState(START);
		m_pPlanet->getTile()->setTileState(GOAL);
		for (int i = 0; i < m_mineNum; ++i)
		{
			m_spawnObject(m_mines[i]);
			m_mines[i]->getTile()->setTileState(IMPASSABLE);
		}

		// first pass
		//m_minePassAdjustment();

		if (m_findShortestPath(m_pShip))
		{
			m_resetGrid();
			m_pShip->clearPath();
			navigable = true; // Try to find a path, if it works, stop trying.
		}
		else
		{
			m_mineNum  = m_mineNum - reductionNum > 0 ? m_mineNum - reductionNum : 0; // and if it doesn't, reduce the number of mines and try again.
			std::cout << "Failed to find path, reducing number of mines by " << reductionNum << " to " << m_mineNum << "." << std::endl;
			++reductionNum;
		}
	}
}

/*
 * This utility function checks for mines that create a dead end
 * and set the middle tile to IMPASSABLE
 */
void PlayScene::m_minePassAdjustment()
{
	for (auto tile : m_pGrid)
	{
		auto mineCount = 0;
		auto nullCount = 0;
		auto neighbours = tile->getNeighbours();
		
		for (auto i = 0; i < neighbours.size(); ++i)
		{
			if (neighbours[i] != nullptr)
			{
				if (neighbours[i]->getTileState() == IMPASSABLE)
				{
					mineCount++;
				}
			}
			else
			{
				nullCount++;
			}

			if (mineCount + nullCount > 2)
			{
				if((tile->getTileState() != START) && (tile->getTileState() != GOAL))
				{
					tile->setTileState(IMPASSABLE);
				}
			}
		}
	}
}

void PlayScene::m_buildEnemies()
{
	for (int index = 0; index < m_enemyNum; ++index)
	{
		auto enemy = new Enemy();
		addChild(enemy);
		m_enemies.push_back(enemy);
	}
}

void PlayScene::m_spawnEnemies()
{
	m_resetGrid();
	for (int i = 0; i < m_enemyNum; ++i)
	{
		m_spawnObject(m_enemies[i]);
		m_enemies[i]->setTargetTile(m_pShip->getTile());
		m_enemies[i]->getTile()->setTileState(CLOSED);
	}
}

void PlayScene::m_eraseEnemies()
{
	for (auto enemy : m_enemies)
	{
		delete enemy;
		enemy = nullptr;
	}
	m_enemies.clear();
	m_enemies.shrink_to_fit();
}

void PlayScene::m_resetGrid()
{
	for (auto tile : m_openList)
	{
		tile->resetTile();
		m_openList.pop_back();
	}

	for (auto tile : m_closedList)
	{
		if (tile->getTileState() != IMPASSABLE)
		{
			tile->resetTile();
		}
		m_closedList.pop_back();
	}
}

void PlayScene::m_buildGrid()
{
	const auto size = Config::TILE_SIZE;
	const auto offset = size * 0.5f;
	
	m_pGrid = std::vector<Tile*>(); // instantiates a structure of type vector<Tile*>

	for (auto row = 0; row < Config::ROW_NUM; ++row)
	{
		for (auto col = 0; col < Config::COL_NUM; ++col)
		{
			// Instantiate the tile
			auto tile = new Tile(glm::vec2(offset + size * col, offset + size * row), 
				glm::vec2(col, row));
			addChild(tile);
			tile->setTileState(UNVISITED);
					
			m_randomizeTileCosts();
			
			// add the tile to the vector
			m_pGrid.push_back(tile);
		}
	}
}

void PlayScene::m_randomizeTileCosts()
{
	for (Tile* tile : m_pGrid)
	{
		// determine the type of tile it should be
		int tileType = rand() % 10;

		switch (tileType)
		{
		case 0:
		case 1:
		case 2:
			tile->setTileCost(1.0f);
			break;
		case 3:
		case 4:
		case 5:
			tile->setTileCost(2.0f);
			break;
		case 6:
		case 7:
			tile->setTileCost(2.5f);
			break;
		case 8:
			tile->setTileCost(3.0f);
			break;
		case 9:
			tile->setTileCost(4.0f);
			break;
		}
	}
}

void PlayScene::m_mapTiles()
{
	for (auto tile : m_pGrid)
	{
		const auto x = tile->getGridPosition().x;
		const auto y = tile->getGridPosition().y;

		if(y != 0)                   { tile->setUp   (m_pGrid[x + ((y - 1) * Config::COL_NUM)]); }
		if(x != Config::COL_NUM - 1) { tile->setRight(m_pGrid[(x + 1) + (y * Config::COL_NUM)]); }
		if(y != Config::ROW_NUM - 1) { tile->setDown (m_pGrid[x + ((y + 1) * Config::COL_NUM)]); }
		if(x != 0)					 { tile->setLeft (m_pGrid[(x - 1) + (y * Config::COL_NUM)]); }
	}
}

int PlayScene::m_spawnObject(PathFindingDisplayObject* object)
{
	Tile* randomTile = nullptr;
	auto randomTileIndex = 0;
	do
	{
		randomTileIndex = int(Util::RandomRange(0, m_pGrid.size() - 1));
		randomTile = m_pGrid[randomTileIndex];
	} while (randomTile->getTileState() != UNVISITED); // search for empty tile


	if (object->getTile() != nullptr)
	{
		object->getTile()->setTileState(UNVISITED);
	}

	object->setPosition(randomTile->getPosition());
	object->setTile(randomTile);

	return randomTileIndex;
}

void PlayScene::m_spawnShip()
{
	m_spawnObject(m_pShip);
	m_resetGrid();
	m_pShip->clearPath();
	m_pShip->getTile()->setTileState(START);
}

void PlayScene::m_spawnPlanet()
{
	m_spawnObject(m_pPlanet);
	m_resetGrid();
	m_pShip->clearPath();
	m_pPlanet->getTile()->setTileState(GOAL);
	m_pShip->setTargetTile(m_pPlanet->getTile());
}

Tile* PlayScene::m_findLowestValueInOpenList()
{
	if (m_openList.size() > 0)
	{
		float lowestValue = m_openList[0]->getTileValue();
		Tile* lowestValueTile = m_openList[0];
		for (Tile* tile : m_openList)
		{
			float f = tile->getTileValue();
			if (f <= lowestValue)
			{
				lowestValueTile = tile;
				lowestValue = f;
			}
		}
		return lowestValueTile;
	}
	return nullptr;
}

bool PlayScene::m_findShortestPath(PathFindingDisplayObject* actor)
{
	m_resetGrid();

	for (auto mine : m_mines)
	{
		mine->getTile()->setTileState(IMPASSABLE);
	} // Why did I need this function? It doesn't make any sense!

	// Add the start node
	float pathCost = 0.0f;
	Tile* currentNode = actor->getTile();
	m_openList.clear();
	m_closedList.clear();
	m_openList.push_back(currentNode);

	// Loop until you find the end
	while (!m_openList.empty())
	{
		// Get the current node
		currentNode = m_findLowestValueInOpenList();					// Let the current node equal the node with the least f value
		for (unsigned int nodeIndex = 0; nodeIndex < m_openList.size(); nodeIndex++) // Remove the current node from the openList
		{
			if (m_openList[nodeIndex]->getGridPosition() == currentNode->getGridPosition())
			{
				m_openList.erase(m_openList.begin() + nodeIndex);
			}
		}
		m_closedList.push_back(currentNode);							// Add the current node to the closedList
		currentNode->setTileState(CLOSED);

		// Found the goal
		if(currentNode == actor->getTargetTile())
		{
			pathCost = currentNode->getTotalCost();
			std::cout << pathCost <<std::endl;
			Tile* nodeToAdd = currentNode;
			while (nodeToAdd != actor->getTile())
			{
				actor->addPathNodeToBack(nodeToAdd);
				nodeToAdd = nodeToAdd->getParentNode();
			}
			return true;
		}

		// Generate children
		const auto neighbours = currentNode->getNeighbours();			// Let the children of the currentNode equal the adjacent nodes
		for (Tile* childNode : neighbours)
		{
			// Ensure that the child is not nullptr
			if (childNode == nullptr)
				{ continue; }

			// Ensure that the child is not impassable
			if (childNode->getTileState() == IMPASSABLE || childNode->getTileState() == CLOSED)
				{ continue; }

			// Ensure that the child is not on the closed list
			if(std::find(m_closedList.begin(), m_closedList.end(), childNode) != m_closedList.end())
				{ continue; }
			
			// Generate the child's f, g, and h values
			float g = childNode->calcTotalCost(currentNode);
			float h = childNode->calcTargetDistance(actor->getTargetTile()->getGridPosition());
			float f = g + h;

			// Check if the child is already in the openList
			if (std::find(m_openList.begin(), m_openList.end(), childNode) != m_openList.end())
			{
				if (childNode->getTotalCost() > g)
				{
					childNode->setParentNode(currentNode);
					childNode->setTotalCost(g);
					childNode->setTargetDistance(h);
					childNode->setTileValue(f);
					childNode->setTileState(OPEN);
				}
			}

			// Add the child to the openList
			childNode->setParentNode(currentNode);
			childNode->setTotalCost(g);
			childNode->setTargetDistance(h);
			childNode->setTileValue(f);
			childNode->setTileState(OPEN);
			m_openList.push_back(childNode);
		}
	}
	return false;
}

void PlayScene::m_selectHeuristic(Heuristic heuristic)
{
	// recalculate grid
	m_heuristic = heuristic;
	auto start = m_pShip->getTile();
	auto goal = m_pPlanet->getTile();
	m_resetGrid();
	//m_computeTileValues();
	start->setTileState(START);
	goal->setTileState(GOAL);
	//m_findShortestPath(m_pShip->getTile());

	// change button colour depending on heuristic chosen
	switch(heuristic)
	{
	case MANHATTAN:
		m_manhattanButtonColour = ImVec4(0.26f, 1.0f, 0.98f, 1.0f);
		m_euclideanButtonColour = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		break;
	case EUCLIDEAN:
		m_manhattanButtonColour = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		m_euclideanButtonColour = ImVec4(0.26f, 1.0f, 0.98f, 1.0f);
		break;
	}
	
}

// ImGui functions ***********************************************

void PlayScene::m_ImGuiKeyMap()
{
	auto& io = ImGui::GetIO();

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
	io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
	io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
	io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
	io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;

	io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
	io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
	io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
	io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
	io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
	io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;
}

void PlayScene::m_ImGuiSetStyle()
{
	auto& style = ImGui::GetStyle();

	style.Alpha = 0.8f;
	style.FrameRounding = 3.0f;
	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void PlayScene::m_updateUI()
{
	// Prepare Window Frame
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow(); // use for debug purposes

	std::string windowString = "Settings ";

	ImGui::Begin(&windowString[0], NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar);

	// set window to top getLeft corner
	ImGui::SetWindowPos(ImVec2(0, 0), true);

	/*************************************************************************************************/
	/* MENU                                                                                          */
	/*************************************************************************************************/

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::Separator();
			ImGui::MenuItem("Exit", NULL, &m_exitApp);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::Separator();
			ImGui::MenuItem("About", NULL, &m_displayAbout);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (m_exitApp)
	{
		TheGame::Instance()->quit();
	}

	if (m_displayAbout)
	{
		ImGui::Begin("About Pathfinding Simulator", &m_displayAbout, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Separator();
		ImGui::Text("Authors:");
		ImGui::Text("Tom Tsiliopoulos ");
		ImGui::Text("Kiera Josie Bacon ");
		ImGui::End();
	}

	/*************************************************************************************************/
	if (ImGui::Button("Respawn Ship [s]"))
	{
		m_spawnShip();
	}

	ImGui::SameLine();

	if (ImGui::Button("Respawn Planet [p]"))
	{
		m_spawnPlanet();
	}

	ImGui::SameLine();

	if (ImGui::Button("Respawn Mines [m]"))
	{
		m_spawnMines();
	}

	ImGui::SameLine();

	if (ImGui::Button("Respawn Enemies [e]"))
	{
		m_spawnEnemies();
	}

	if (ImGui::Button("Randomize Tile Costs [r]"))
	{
		m_randomizeTileCosts();
	}

	ImGui::SameLine();

	if (ImGui::Button("Find Shortest Path [f]"))
	{
		m_findShortestPath(m_pShip);
	}

	ImGui::SameLine();

	if (ImGui::Button("End Turn (Move Along Path) [space]"))
	{
		endTurn();
	}

	if(ImGui::CollapsingHeader("Heuristic Options"))
	{
		ImGui::PushStyleColor(ImGuiCol_Button, m_manhattanButtonColour);
		if (ImGui::Button("Manhattan Distance"))
		{
			m_selectHeuristic(MANHATTAN);
		}
		ImGui::PopStyleColor();

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, m_euclideanButtonColour);
		if (ImGui::Button("Euclidean Distance"))
		{
			m_selectHeuristic(EUCLIDEAN);
		}
		ImGui::PopStyleColor();
	}

	if(ImGui::SliderInt("Number of Mines", &m_mineNum, 1, 150))
	{
		m_eraseMines();
		m_buildMines();
		m_spawnMines();
	}

	if (ImGui::SliderInt("Number of Enemies", &m_enemyNum, 1, 150))
	{
		m_eraseEnemies();
		m_buildEnemies();
		m_spawnEnemies();
	}

	if(ImGui::CollapsingHeader("Visibility Options"))
	{
		if (ImGui::Checkbox("Debug", &m_displayUI)) {}
		ImGui::SameLine();
		if (ImGui::Checkbox("Tiles", &m_tilesVisible)) {}
		ImGui::SameLine();
		if(ImGui::Checkbox("Ship", &m_shipVisible)) {}
		ImGui::SameLine();
		if (ImGui::Checkbox("Planet", &m_planetVisible)) {}
		ImGui::SameLine();
		if (ImGui::Checkbox("Mines", &m_minesVisible)) {}
		ImGui::SameLine();
		if (ImGui::Checkbox("Enemies", &m_enemiesVisible)) {}
	}
	

	// Main Window End
	ImGui::End();
}

/*** SCENE FUNCTIONS ***/
void PlayScene::m_resetAll()
{
	
}

void PlayScene::start()
{
	Game::Instance()->setScore(0.0f);

	// setup default heuristic options
	m_heuristic = MANHATTAN;
	m_manhattanButtonColour = ImVec4(0.26f, 1.0f, 0.98f, 0.40f);
	m_euclideanButtonColour = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	
	m_buildGrid();
	m_mapTiles();

	// instantiate ship and add it to the DisplayList
	m_pShip = new Ship();
	addChild(m_pShip);
	m_spawnShip();

	// instantiate planet and add it to the DisplayList
	m_pPlanet = new Planet();
	addChild(m_pPlanet);
	m_spawnPlanet();

	m_buildMines();
	m_spawnMines();

	m_buildEnemies();
	m_spawnEnemies();

	// set up the labels
	const SDL_Color black = { 0, 0, 0, 255 };
	m_pTurnLabel = new Label("Turn: " + m_turnNum, "Consolas", 24, black, glm::vec2(10.0f, 25.0f), TTF_STYLE_NORMAL, false);
	m_pTurnLabel->setParent(this);
	addChild(m_pTurnLabel);

	m_pScoreLabel = new Label("Score: " + std::to_string(Game::Instance()->getScore()), "Consolas", 24, black, glm::vec2(10.0f, 45.0f), TTF_STYLE_NORMAL, false);
	m_pScoreLabel->setParent(this);
	addChild(m_pScoreLabel);

	m_pPtsLabel = new Label("Action Pts: " + std::to_string(m_pShip->getCurrentPts()), "Consolas", 24, black, glm::vec2(10.0f, 65.0f), TTF_STYLE_NORMAL, false);
	m_pPtsLabel->setParent(this);
	addChild(m_pPtsLabel);
}

void PlayScene::endTurn()
{
	++m_turnNum;

	// Move the player
	m_pShip->moveAlongPath();
	m_pShip->newTurn();

	// Update the score and labels
	Game::Instance()->setScore(Game::Instance()->getScore() + m_pShip->getTile()->getTileCost());
	m_pTurnLabel->setText("Turn: " + std::to_string(getTurnNum()));
	
	std::ostringstream scoreString;
	scoreString << "Score: " << std::fixed << std::setprecision(0) << Game::Instance()->getScore();
	m_pScoreLabel->setText(scoreString.str());
	
	std::ostringstream ptsString;
	ptsString << "Action Pts: " << std::fixed << std::setprecision(0) << m_pShip->getCurrentPts();
	m_pPtsLabel->setText(ptsString.str());

	// Move the enemies
	for (unsigned int i = 0; i < m_enemies.size(); i++)
	{
		m_enemies[i]->setTargetTile(m_pShip->getTile());
		if (m_findShortestPath(m_enemies[i]))
		{
			m_enemies[i]->moveAlongPath();
			m_enemies[i]->newTurn();
		}
	}

	// Clear the enemies' paths from the screen
	m_resetGrid();

	// Check for victory or loss
	if (m_pShip->getTile()->getGridPosition() == m_pPlanet->getTile()->getGridPosition())
	{
		std::cout << "You've reached the planet! Victory!" << std::endl;
		Game::Instance()->changeSceneState(SceneState::END_SCENE);
	}
	else
	{
		for (unsigned int i = 0; i < m_enemies.size(); i++)
		{
			if (m_enemies[i]->getTile()->getGridPosition() == m_pShip->getTile()->getGridPosition())
			{
				std::cout << "You've been hit by an enemy! Defeat!" << std::endl;
				Game::Instance()->changeSceneState(SceneState::DEFEAT_SCENE);
			}
		}
	}

}

const unsigned int PlayScene::getTurnNum()
{
	return m_turnNum;
}

PlayScene::PlayScene()
{
	PlayScene::start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	if (m_displayUI)
	{
		for (auto tile : m_pGrid)
		{
			tile->drawFrame();
		}
	}

	if (m_tilesVisible)
	{
		// Draw the tiles
	}

	if(m_planetVisible)
	{
		m_pPlanet->draw();
	}
	
	if(m_minesVisible)
	{
		for (auto mine : m_mines)
		{
			mine->draw();
		}
	}

	if (m_enemiesVisible)
	{
		for (auto enemy : m_enemies)
		{
			enemy->draw();
		}
	}

	if(m_shipVisible)
	{
		m_pShip->draw();
	}

	if (m_labelsVisible)
	{
		m_pPtsLabel->draw();
		m_pScoreLabel->draw();
	}

	// ImGui Rendering section - DO NOT MOVE OR DELETE
	if (m_displayUI)
	{
		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());
		SDL_SetRenderDrawColor(TheGame::Instance()->getRenderer(), 255, 255, 255, 255);
	}
}

void PlayScene::update()
{
	/*m_pTile->update();
	m_pShip->update();*/

	if (m_displayUI)
	{
		m_updateUI();
	}

}

void PlayScene::clean()
{
	std::cout << "PlayScene Clean Called" << std::endl;
	delete m_pShip;
	delete m_pPlanet;

	for (auto tile : m_pGrid)
	{
		delete tile;
		tile = nullptr;
	}
	m_pGrid.clear();
	m_pGrid.resize(0);
	m_pGrid.shrink_to_fit();

	for (auto mine : m_mines)
	{
		delete mine;
		mine = nullptr;
	}
	m_mines.clear();
	m_mines.resize(0);
	m_mines.shrink_to_fit();

	m_openList.clear();
	m_openList.resize(0);
	m_openList.shrink_to_fit();

	m_closedList.clear();
	m_closedList.resize(0);
	m_openList.shrink_to_fit();

	removeAllChildren();
}

void PlayScene::handleEvents()
{
	auto& io = ImGui::GetIO();
	auto wheel = 0;

	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			TheGame::Instance()->quit();
			break;
		case SDL_MOUSEMOTION:
			m_mousePosition.x = event.motion.x;
			m_mousePosition.y = event.motion.y;
			break;
		case SDL_MOUSEWHEEL:
			wheel = event.wheel.y;
			break;
		case SDL_TEXTINPUT:
			io.AddInputCharactersUTF8(event.text.text);
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				TheGame::Instance()->quit();
				break;
			case SDLK_1:
				TheGame::Instance()->changeSceneState(SceneState::START_SCENE);
				break;
			case SDLK_2:
				TheGame::Instance()->changeSceneState(SceneState::END_SCENE);
				break;
			case SDLK_BACKQUOTE:
				m_displayUI = (m_displayUI) ? false : true;
				break;
			case SDLK_f:
				m_findShortestPath(m_pShip);
				break;
			case SDLK_e:
				m_spawnEnemies();
				break;
			case SDLK_m:
				m_spawnMines();
				break;
			case SDLK_p:
				m_spawnPlanet();
				break;
			case SDLK_r:
				m_randomizeTileCosts();
				//m_resetAll();
				break;
			case SDLK_s:
				m_spawnShip();
				break;
			case SDLK_SPACE:
				endTurn();
				break;

				/************************************************************************/
			case SDLK_w:
				
				break;
			
			case SDLK_a:
				
				break;
			case SDLK_d:
				
				break;
			default:
				
				break;
			}
			{
				const int key = event.key.keysym.scancode;
				IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
				io.KeysDown[key] = (event.type == SDL_KEYDOWN);
				io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
				io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
				io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
				io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_w:
				
				break;

			case SDLK_s:
				
				break;

			case SDLK_a:
				
				break;
			case SDLK_d:
				
				break;
			default:
				
				break;
			}
			{
				const int key = event.key.keysym.scancode;
				IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
				io.KeysDown[key] = (event.type == SDL_KEYDOWN);
				io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
				io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
				io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
				io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
			}
			break;
		default:
			
			break;
		}
	}

	io.DeltaTime = 1.0f / 60.0f;
	int mouseX, mouseY;
	const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
	io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
	io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
	io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
	io.MouseWheel = static_cast<float>(wheel);

	io.DisplaySize.x = 1280;
	io.DisplaySize.y = 720;

	m_ImGuiKeyMap();
	m_ImGuiSetStyle();
}
