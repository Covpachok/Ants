#ifndef ANTS_ANTCOLONY_HPP
#define ANTS_ANTCOLONY_HPP

#include <cstdint>
#include <vector>

#include "Timer.hpp"

#include "Nest.hpp"
#include "PheromoneMap.hpp"
#include "Aliases.hpp"
#include "Ant.hpp"

#include "TileMap.hpp"

class AntColony
{
public:
	AntColony(AntColonyId id, const Vector2 &antsSpawnPos);

	void Update(TileMap &tileMap);

	void SpawnAnt(const Vector2 &pos);
	void RemoveAnt(AntId id);

	AntColonyId GetId() const { return m_id; }
	void DrawAnts() const;
	void DrawPheromones() const;

private:
	void UpdateTimers();

private:
	AntColonyId m_id;

	size_t m_antsAmount;
	size_t m_maxAntsAmount;

	std::vector<std::unique_ptr<Ant>> m_ants;
	std::unique_ptr<PheromoneMap>     m_pheromoneMap;

	Timer m_pheromoneSpawnTimer;
	Timer m_fovCheckTimer;
	Timer m_antDeathTimer;
};


#endif //ANTS_ANTCOLONY_HPP
