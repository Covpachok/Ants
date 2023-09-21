#ifndef ANTS_COLONIESMANAGER_HPP
#define ANTS_COLONIESMANAGER_HPP

#include "AntColony.hpp"

#include <vector>
#include <cassert>

class ColoniesManager
{
	const size_t k_maxColoniesAmount = 1;
	const size_t k_maxNestsAmount    = 1;
public:
	ColoniesManager(TileMap &tileMap);

	void CreateColony(const Vector2 &pos);
	void CreateNest(const IntVec2 &pos, TileMap &tileMap, AntColony *colony);

	std::vector<std::unique_ptr<AntColony>> &GetColonies() { return m_colonies; }

private:
	std::vector<std::unique_ptr<AntColony>> m_colonies;
	std::vector<std::unique_ptr<Nest>>      m_nests;

	AntColonyId s_nextColonyId = 0;
	NestId      s_nextNestId   = 0;
};


#endif //ANTS_COLONIESMANAGER_HPP
