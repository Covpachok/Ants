#ifndef ANTS_VALUETABLE_HPP
#define ANTS_VALUETABLE_HPP

#include <raylib.h>
#include <cassert>
#include <string>

constexpr int k_cellsAmount = 3;

struct AntsValueTable
{
	float antMovementSpeed = 40;
	float antRotationSpeed = 7;

	float antRandomAngle = 0.3;
	int   antFovRange    = 12;

	float foodPheromoneStrengthLoss = 0.005;
	float homePheromoneStrengthLoss = 0.005;

	float foodPheromoneIntensity = 128;
	float homePheromoneIntensity = 128;

	float pheromoneSpawnDelay = 0.25f;

	int deviationChance = 2;

	Color antDefaultColor  = {128, 128, 255, 128};
	Color antWithFoodColor = {128, 255, 128, 128};
};

struct WorldValueTable
{
	enum MapGenSettings
	{
		None, FoodOnly, WallsOnly, FoodAndWalls, Amount
	};

	Color foodPheromoneColor = {0, 255, 0, 0};
	Color homePheromoneColor = {0, 0, 255, 0};

	Color cellColors[k_cellsAmount] = {{0,   0,   0,   255},
	                                   {0,   255, 64,  255},
	                                   {128, 128, 128, 255}};

	// Why? Because.
	int cellDefaultAmount[k_cellsAmount] = {0, 30, 0};

	float homePheromoneEvaporationRate = 0.4f;
	float foodPheromoneEvaporationRate = 0.4f;

	int homeRadius = 5;

	bool centeredHomePos = true;
	int  homePos[2]      = {0, 0};

	int antsAmount = 1000;

//	bool  shouldGenerateMap = true;
	float mapGenNoiseScale    = 8.f;
	int   mapGenNoiseBlur     = 2;
	int   mapGenNoiseContrast = 8;

	MapGenSettings mapGenSettings = MapGenSettings::FoodAndWalls;

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

	void Save(const std::string &filename);
	void Load(const std::string &filename);

	void Reset()
	{
		m_antsTable  = AntsValueTable();
		m_worldTable = WorldValueTable();
	};

private:
	AntsValueTable  m_antsTable;
	WorldValueTable m_worldTable;
};

#endif //ANTS_VALUETABLE_HPP
