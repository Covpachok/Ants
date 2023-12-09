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

struct AntColonySettings
{
	size_t coloniesAmount  = 1; // didn't work yet
	int    antsStartAmount = 1000;
	int    antsMaxAmount   = 2500;

	bool dynamicLife    = false;
	int  antDeathDelay  = 25;
	int  foodToSpawnAnt = 10;

	int nestSize = 5;
};

class AntColony
{
public:
	AntColony(AntColonyId id, const Vector2 &antsSpawnPos);

	void Update(TileMap &tileMap);

	void SpawnAnt(const Vector2 &pos);
	void RemoveAnt(AntId id);

	void DrawAnts() const;
	void DrawPheromones() const;

	AntColonyId GetId() const { return m_id; }
	size_t GetAntsAmount() const { return m_antsAmount; }

private:
	void UpdateTimers();

	void OnAntsAmountChanged();

private:
	AntColonyId m_id;

	Vector2 m_initialAntsSpawnPos;

	size_t m_antsAmount;
	size_t m_maxAntsAmount;

	std::vector<std::unique_ptr<Ant>> m_ants;
	std::unique_ptr<PheromoneMap>     m_pheromoneMap;

	float m_antDeathDelay;

	// Planned on remaking simulation to be data oriented,
	// for potentially improved performance, but nah
//	Timer m_pheromoneSpawnTimer;
//	Timer m_fovCheckTimer;

	Timer m_antDeathTimer;

	bool m_dynamicLife;
};


#endif //ANTS_ANTCOLONY_HPP
