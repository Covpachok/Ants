//
// Created by heylc on 15.08.2023.
//

#ifndef ANTS_VALUETABLE_HPP
#define ANTS_VALUETABLE_HPP

#include <raylib.h>

struct AntsValueTable
{
	float antRotationSpeed = 12;
	float antMovementSpeed = 40;

	float antRandomAngle = 0.3;
	int   antFovRange    = 8;

	float antFoodStrengthLoss = 0.015;
	float antHomeStrengthLoss = 0.005;

	float foodPheromoneIntensity = 128;
	float homePheromoneIntensity = 128;

	float pheromoneSpawnDelay = 0.25f;

	// VERY IMPORTANT VALUE 0.05
	float antPheromoneCheckDelay = 0.05f; // DO NOT CHANGE


	Color antDefaultColor  = {128, 128, 255, 128};
	Color antWithFoodColor = {128, 255, 128, 128};
	Color *antColorPtr[2]  = {&antDefaultColor, &antWithFoodColor};
};

struct WorldValueTable
{
	Color foodPheromoneColor = {32, 128, 32, 0};
	Color homePheromoneColor = {32, 32, 128, 0};

	Color cellColors[2] = {{0, 0,   0, 255},
	                       {0, 255, 64, 255}};

	int cellDefaultAmount[2] = {0, 30};

	int worldWidth;
	int worldHeight;

	double homePheromoneEvaporationRate = 2;
	double foodPheromoneEvaporationRate = 2;

	int antsAmount = 1000;
};

class ValueTable
{
public:
	const AntsValueTable &GetAntsTable() const { return m_antsTable; };
	const WorldValueTable &GetWorldTable() const { return m_worldTable; };

	AntsValueTable &GetMutableAntsTable() { return m_antsTable; }
	WorldValueTable &GetMutableWorldTable() { return m_worldTable; }

private:
	AntsValueTable  m_antsTable;
	WorldValueTable m_worldTable;
};

inline ValueTable g_valueTable;

#endif //ANTS_VALUETABLE_HPP
