//
// Created by heylc on 15.08.2023.
//

#ifndef ANTS_VALUETABLE_HPP
#define ANTS_VALUETABLE_HPP

#include <raylib.h>

constexpr int k_cellsAmount = 3;

struct AntsValueTable
{
	float antMovementSpeed = 40;
	float antRotationSpeed = 12;

	float antRandomAngle = 0.3;
	int   antFovRange    = 8;

	float foodPheromoneStrengthLoss = 0.025;
	float homePheromoneStrengthLoss = 0.025;

	float foodPheromoneIntensity = 128;
	float homePheromoneIntensity = 128;

	float pheromoneSpawnDelay = 0.25f;

	// VERY IMPORTANT VALUE 0.05
	float antPheromoneCheckDelay = 0.05f; // DO NOT CHANGE

	Color antDefaultColor  = {128, 128, 255, 128};
	Color antWithFoodColor = {128, 255, 128, 128};
};

struct WorldValueTable
{
	Color foodPheromoneColor = {32, 128, 32, 0};
	Color homePheromoneColor = {32, 32, 128, 0};

	Color cellColors[k_cellsAmount] = {{0,   0,   0,   255},
	                                   {0,   255, 64,  255},
	                                   {128, 128, 128, 255}};

	// Why? Because.
	int cellDefaultAmount[k_cellsAmount] = {0, 30, 0};

	float homePheromoneEvaporationRate = 1;
	float foodPheromoneEvaporationRate = 1;

	int homeRadius = 5;

	bool centeredHomePos = true;
	int  homePos[2]      = {0, 0};

	int antsAmount = 1000;

	bool  shouldGenerateMap = true;
	float mapGenNoiseScale  = 8.f;
	int   mapGenNoiseBlur   = 2;

	int mapGenFoodLowThreshold  = 0;
	int mapGenFoodHighThreshold = 64;

	int mapGenWallLowThreshold  = 160;
	int mapGenWallHighThreshold = 255;
};

class ValueTable
{
public:
	const AntsValueTable &GetAntsTable() const { return m_antsTable; };
	const WorldValueTable &GetWorldTable() const { return m_worldTable; };

	AntsValueTable &GetMutableAntsTable() { return m_antsTable; }
	WorldValueTable &GetMutableWorldTable() { return m_worldTable; }

	ValueTable &operator=(ValueTable &&other) = default;
	ValueTable &operator=(const ValueTable &other) = default;

private:
	AntsValueTable  m_antsTable;
	WorldValueTable m_worldTable;
};

inline ValueTable       g_valueTable;
inline const ValueTable g_defaultValueTable;

#endif //ANTS_VALUETABLE_HPP
