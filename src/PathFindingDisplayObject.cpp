#include "PathFindingDisplayObject.h"
#include "Tile.h"

PathFindingDisplayObject::PathFindingDisplayObject(): m_currentTile(nullptr), m_initialPts(0.0f)
{
}

PathFindingDisplayObject::~PathFindingDisplayObject()
= default;

Tile* PathFindingDisplayObject::getTile() const
{
	return m_currentTile;
}

void PathFindingDisplayObject::setTile(Tile* new_tile)
{
	m_currentTile = new_tile;
}

void PathFindingDisplayObject::addPathNodeToBack(Tile* node)
{
	m_path.push_back(node);
}

float PathFindingDisplayObject::getCurrentPts()
{
	return m_currentPts;
}

void PathFindingDisplayObject::moveAlongPath()
{
	std::cout << "pts" << m_currentPts << std::endl;
	if (!m_path.empty())
	{
		while (!m_path.empty() && m_usePts(m_path.back()->getTileCost()) != false)
		{
			setTile(m_path.back());
			setPosition(getTile()->getPosition());
			m_path.pop_back();
		}
	}
	else
	{
		m_path.clear();
		m_path.shrink_to_fit();
	}
}

std::vector<Tile*>& PathFindingDisplayObject::getPath()
{
	return m_path;
}

void PathFindingDisplayObject::clearPath()
{
	m_path.clear();
	m_path.shrink_to_fit();
}

Tile* PathFindingDisplayObject::getTargetTile()
{
	return m_targetTile;
}

void PathFindingDisplayObject::setTargetTile(Tile* target)
{
	m_targetTile = target;
}

void PathFindingDisplayObject::newTurn()
{
	m_currentPts += m_initialPts;
}

void PathFindingDisplayObject::setInitialPts(float pts)
{
	m_initialPts = pts;
}

float PathFindingDisplayObject::getInitialPtr()
{
	return m_initialPts;
}

void PathFindingDisplayObject::setCurrentPts(float pts)
{
	m_currentPts = pts;
}

bool PathFindingDisplayObject::m_usePts(float amount)
{
	if (amount <= m_currentPts)
	{
		m_currentPts -= amount;
		return true;
	}
	return false;
}
