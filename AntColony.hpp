#ifndef ANTS_ANTCOLONY_HPP
#define ANTS_ANTCOLONY_HPP

#include <cstdint>
#include <vector>

#include "Nest.hpp"
#include "TileMap.hpp"
#include "Timer.hpp"
#include "PheromoneMap.hpp"

class Ant;

using AntColonyId = uint32_t;

class AntColony
{
	inline static int s_colonyIds = 1;

public:
	AntColony(const IntVec2 &nestPos);

	void Update(const TileMap &tileMap);

	void SpawnAnt(const Nest &spawnNest);

private:
	AntColonyId m_id;

	size_t m_antsAmount;
	size_t m_antsMaxAmount;

	std::vector<std::unique_ptr<Ant>> m_ants;

	std::unique_ptr<PheromoneMap> m_foodPheromoneMap;
	std::unique_ptr<PheromoneMap> m_nestPheromoneMap;
	std::unique_ptr<PheromoneMap> m_lostPheromoneMap;

	Timer m_pheromoneSpawnTimer;
	Timer m_fovCheckTimer;
};


#endif //ANTS_ANTCOLONY_HPP
