#include "Tile.h"
#include <iomanip>
#include <sstream>
#include "Game.h"
#include "Util.h"


Tile::Tile(glm::vec2 world_position, glm::vec2 grid_position) :
	m_gridPosition(grid_position), m_pParentNode(nullptr), m_tilePathColour(Tile::Path::NONE)
{
	TheTextureManager::Instance()->load("../Assets/textures/tile.png",
		"tile", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("../Assets/textures/tallGrass.png",
		"tallGrass", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("../Assets/textures/shortGrass.png",
		"shortGrass", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("../Assets/textures/mediumGrass.png",
		"mediumGrass", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("../Assets/textures/sand.png",
		"sand", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("../Assets/textures/PathfindingDot_Green.png",
		"PathfindingDot_Green", TheGame::Instance()->getRenderer());
	TheTextureManager::Instance()->load("../Assets/textures/PathfindingDot_Red.png",
		"PathfindingDot_Red", TheGame::Instance()->getRenderer());

	auto size = TheTextureManager::Instance()->getTextureSize("tile");
	//auto size = TheTextureManager::Instance()->getTextureSize("grass");
	setWidth(size.x);
	setHeight(size.y);
	setPosition(world_position);

	std::ostringstream tempLabel;
	//tempLabel << std::fixed << std::setprecision(1) <<  m_tileValue;
	//tempLabel << "-";
	//auto labelstring = tempLabel.str();

	SDL_Color black{ 0, 0, 0, 255 };

	auto closedOpenLabelPosition	= glm::vec2(getPosition().x + 10, getPosition().y - 10);
	auto valueLabelPosition			= glm::vec2(getPosition().x - 10, getPosition().y - 10);
	auto costLabelPosition			= glm::vec2(getPosition().x + 10, getPosition().y + 10);
	auto heuristicLabelPosition		= glm::vec2(getPosition().x - 10, getPosition().y + 10);
	m_pClosedOpenLabel				= new Label("-", "Consolas", 10, black, closedOpenLabelPosition);
	m_pValueLabel					= new Label("-", "Consolas", 10, black, valueLabelPosition, true);
	m_pCostLabel					= new Label("-", "Consolas", 10, black, costLabelPosition, true);
	m_pHeuristicLabel				= new Label("-", "Consolas", 10, black, heuristicLabelPosition, true);

	m_pNeighbours = { nullptr, nullptr, nullptr, nullptr };
	m_heuristic = MANHATTAN;
}

Tile::~Tile()
{
	delete m_pValueLabel;
	delete m_pClosedOpenLabel;
	m_pNeighbours.clear();
	m_pNeighbours.resize(0);
	m_pNeighbours.shrink_to_fit();
}

void Tile::draw()
{
	const int xComponent = getPosition().x;
	const int yComponent = getPosition().y;
	float tileCost = getTileCost();
	tileCost = tileCost == 0 ? 1.0f : tileCost;

	if (tileCost <= 1.0f)
	{
		TheTextureManager::Instance()->draw("shortGrass", xComponent, yComponent,
			TheGame::Instance()->getRenderer(), 0, 255, true);
	}
	else if (tileCost <= 2.0f)
	{
		TheTextureManager::Instance()->draw("mediumGrass", xComponent, yComponent,
			TheGame::Instance()->getRenderer(), 0, 255, true);
	}
	else if (tileCost <= 3.0f)
	{
		TheTextureManager::Instance()->draw("tallGrass", xComponent, yComponent,
			TheGame::Instance()->getRenderer(), 0, 255, true);
	}
	else
	{
		TheTextureManager::Instance()->draw("sand", xComponent, yComponent,
			TheGame::Instance()->getRenderer(), 0, 255, true);
	}
}

void Tile::update()
{
}

void Tile::clean()
{
	
}

void Tile::drawFrame(int alpha)
{			    
	const int x = getPosition().x - Config::TILE_SIZE * 0.5;
	const int y = getPosition().y - Config::TILE_SIZE * 0.5;
	const int w = Config::TILE_SIZE;
	const int h = Config::TILE_SIZE;
	const SDL_Rect rect = { x, y, w, h };

	float totalCost = getTotalCost();
	totalCost = totalCost == 0 ? 1.0f : totalCost;
	float tileCost = getTileCost();
	tileCost = tileCost == 0 ? 1.0f : tileCost;

	int gB = (4 / tileCost) * 64 > 255 ? 255 : (4 / tileCost) * 64;
	int rB = tileCost * 64 > 255 ? 255 : tileCost * 64;
	int rW = totalCost * 10 > 255 ? 255 : totalCost * 10;
	int gW = (2.5 / totalCost) * 10 > 255 ? 255 : (25 / totalCost) * 10;
	if (getTileState() == IMPASSABLE)
	{
		rW = 255;
		gW = 0;
	}
	
	SDL_SetRenderDrawBlendMode(TheGame::Instance()->getRenderer(), SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(TheGame::Instance()->getRenderer(), rB, gB, 0, alpha);
	SDL_RenderFillRect(TheGame::Instance()->getRenderer(), &rect);
	SDL_SetRenderDrawColor(TheGame::Instance()->getRenderer(), rW, gW, 0, 255);
	SDL_RenderDrawRect(TheGame::Instance()->getRenderer(), &rect);
}

void Tile::drawLabels()
{
	m_pClosedOpenLabel->draw();
	m_pValueLabel->draw();
	m_pHeuristicLabel->draw();
	m_pCostLabel->draw();
}

void Tile::drawDots(int alpha)
{
	const int xComponent = getPosition().x;
	const int yComponent = getPosition().y;

	if (m_tilePathColour == Tile::Path::GREEN)
	{
		TheTextureManager::Instance()->draw("PathfindingDot_Green", xComponent, yComponent,
			TheGame::Instance()->getRenderer(), 0, alpha, true);
	}
	else if (m_tilePathColour == Tile::Path::RED)
	{
		TheTextureManager::Instance()->draw("PathfindingDot_Red", xComponent, yComponent,
			TheGame::Instance()->getRenderer(), 0, alpha, true);
	}
}

Tile * Tile::getUp()
{
	return m_pNeighbours[UP];
}

Tile * Tile::getDown()
{
	return m_pNeighbours[DOWN];
}

Tile * Tile::getRight()
{
	return m_pNeighbours[RIGHT];
}

Tile * Tile::getLeft()
{
	return m_pNeighbours[LEFT];
}

void Tile::setUp(Tile * tile)
{
	m_pNeighbours[UP] = tile;
}

void Tile::setDown(Tile * tile)
{
	m_pNeighbours[DOWN] = tile;
}

void Tile::setRight(Tile * tile)
{
	m_pNeighbours[RIGHT] = tile;
}

void Tile::setLeft(Tile * tile)
{
	m_pNeighbours[LEFT] = tile;
}

void Tile::setTileState(const TileState state)
{
	m_tileState = state;

	switch(state)
	{
	case NO_PATH:
		setTileStateLabel("N");
		break;
	case OPEN:
		setTileStateLabel("O");
		break;
	case CLOSED:
		setTileStateLabel("C");
		break;
	case START:
		setTileStateLabel("S");
		break;
	case GOAL:
		setTileStateLabel("G");
		m_tileValue = 0;
		break;
	case UNVISITED:
		setTileStateLabel("-");
		break;
	case IMPASSABLE:
		setTileStateLabel("I");
		break;
	default:
		std::cout << "a state that has not been defined" << std::endl;
		break;
	}

	if (state == GOAL)
	{
		m_tileValue = 0;
	}
}

TileState Tile::getTileState() const
{
	return m_tileState;
}

float Tile::calcTargetDistance(const glm::vec2 goal_location)
{
	//m_goalLocation = goal_location;

	// declare heuristic;
	float h = 0.0f;

	switch(m_heuristic)
	{
		case EUCLIDEAN:
			//euclidean distance heuristic
			h = Util::distance(getGridPosition(), goal_location);
			break;
		case MANHATTAN:
			//manhattan distance heuristic
			h = abs(getGridPosition().x - goal_location.x) +
				abs(getGridPosition().y - goal_location.y);
			break;
		default:
			break;
	}

	return h;
}

void Tile::setTargetDistance(float distance)
{
	m_targetDist = distance;
	std::ostringstream tempLabel;
	if (m_targetDist == 0.0f)
	{
		tempLabel << "-";
	}
	else
	{
		tempLabel << std::fixed << std::setprecision(0) << m_targetDist;
	}
	const auto labelstring = tempLabel.str();
	m_pHeuristicLabel->setText(labelstring);
}

float Tile::getTargetDistance() const
{
	return m_targetDist;
}

glm::vec2 Tile::getGridPosition() const
{
	return m_gridPosition;
}

void Tile::setTileCost(float cost)
{
	m_tileCost = cost;
}

float Tile::getTileCost() const
{
	return m_tileCost;
}

void Tile::setTotalCost(float cost)
{
	m_totalCost = cost;
	std::ostringstream tempLabel;
	if (m_totalCost == 0.0f)
	{
		tempLabel << "-";
	}
	else
	{
		tempLabel << std::fixed << std::setprecision(0) << m_totalCost;
	}
	const auto labelstring = tempLabel.str();
	m_pCostLabel->setText(labelstring);
}

float Tile::getTotalCost() const
{
	return m_totalCost;
}

float Tile::calcTotalCost(Tile* parent)
{
	float totalCost = 0.0f;
	Tile* tile = parent;
	while (tile != nullptr)
	{
		totalCost += tile->getTileCost();
		tile = tile->getParentNode();
	}
	return totalCost;
}

float Tile::getTileValue() const
{
	float g = getTotalCost();
	float h = getTargetDistance();
	float f = g + h;
	return f;
}

void Tile::setTileValue(const float new_value)
{
	m_tileValue = new_value;
	std::ostringstream tempLabel;
	if (m_tileValue == 0.0f)
	{
		tempLabel << "-";
	}
	else
	{
		tempLabel << std::fixed << std::setprecision(0) << m_tileValue;
	}
	const auto labelstring = tempLabel.str();
	m_pValueLabel->setText(labelstring);
}

void Tile::setTileStateLabel(const std::string& closed_open) const
{
	m_pClosedOpenLabel->setText(closed_open);

	const SDL_Color blue = { 0, 0, 255, 255 };
	m_pClosedOpenLabel->setColour(blue);
}

void Tile::setParentNode(Tile* parent)
{
	m_pParentNode = parent;
}

Tile* Tile::getParentNode() const
{
	return m_pParentNode;
}

void Tile::setTilePathColour(Tile::Path colour)
{
	if (!(m_tilePathColour == Tile::Path::GREEN && colour == Tile::Path::RED))
	{
		m_tilePathColour = colour;
	}
}

void Tile::resetTile()
{
	setTileState(UNVISITED);
	setTargetDistance(0.0f);
	setTotalCost(0.0f);
	setTileValue(0.0f);
	setParentNode(nullptr);
}

std::vector<Tile*> Tile::getNeighbours() const
{
	return m_pNeighbours;
}

void Tile::setHeuristic(const Heuristic heuristic)
{
	m_heuristic = heuristic;
}

void Tile::displayTile()
{
	std::cout << "+------------------------------->" << std::endl;
	std::cout << "+-                             ->" << std::endl;
	
	if(getUp() != nullptr)
	{
		if(getUp()->getTileState() != IMPASSABLE)
		{
			std::cout << "+-         U: " << getUp()->getTileValue() << "             ->" << std::endl;
		}
		else
		{
			std::cout << "+-         U: mine             ->" << std::endl;
		}
	}
	else
	{
		std::cout << "+-         U: nptr             ->" << std::endl;
	}

	if(getLeft() != nullptr)
	{
		if (getLeft()->getTileState() != IMPASSABLE)
		{
			std::cout << "+- L: " << getLeft()->getTileValue();
		}
		else
		{
			std::cout << "+- L: mine";
		}
	}
	else
	{
		std::cout << "+- L: nptr";
	}
	
	std::cout << " T: " << getTileValue();

	if(getRight() != nullptr)
	{
		if (getRight()->getTileState() != IMPASSABLE)
		{
			std::cout << " R: " << getRight()->getTileValue() << "     ->" << std::endl;
		}
		else
		{
			std::cout << " R: mine    ->" << std::endl;
		}
	}
	else
	{
		std::cout << " R: nptr    ->" << std::endl;
	}

	if(getDown() != nullptr)
	{
		if (getDown()->getTileState() != IMPASSABLE)
		{
			std::cout << "+-         D: " << getDown()->getTileValue() << "             ->" << std::endl;
		}
		else
		{
			std::cout << "+-         D: mine             ->" << std::endl;
		}
	}
	else
	{
		std::cout << "+-         D: nptr             ->" << std::endl;
	}
	
	std::cout << "+-                             ->" << std::endl;
	std::cout << "+------------------------------->" << std::endl;
}
