#ifndef ANTS_SETTINGS_HPP
#define ANTS_SETTINGS_HPP

#include <raylib.h>
#include <cassert>
#include <string>
#include <vector>

constexpr int k_tilesAmount = 3;

struct AntsSettings
{
	float antMovementSpeed = 40;
	float antRotationSpeed = 10;

	float antRandomRotation = 0.3;
	int   antFovRange       = 12;

	float foodPheromoneStrengthLoss = 0.005;
	float homePheromoneStrengthLoss = 0.005;

	float foodPheromoneIntensity = 128;
	float homePheromoneIntensity = 128;

	float pheromoneSpawnDelay = 0.25f;

	float deviationChance = 0.001;

	Color antDefaultColor  = {128, 128, 255, 128};
	Color antWithFoodColor = {128, 255, 128, 128};
};

struct WorldSettings
{
	int mapWidth  = 800;
	int mapHeight = 100;
	float screenToMapRatio = 3.f;

	enum MapGenSettings
	{
		None, FoodOnly, WallsOnly, FoodAndWalls, Amount
	};

	Color foodPheromoneColor = {0, 255, 0, 0};
	Color homePheromoneColor = {0, 0, 255, 0};

	Color tileColors[k_tilesAmount] = {{0,   0,   0,   0},
	                                   {0,   255, 64,  255},
	                                   {128, 128, 128, 255}};

	// Why? Because.
	int tileDefaultAmount[k_tilesAmount] = {0, 30, 0};

	float homePheromoneEvaporationRate = 0.008f;
	float foodPheromoneEvaporationRate = 0.008f;

	int homeRadius = 5;

	bool centeredHomePos = true;
	int  homePos[2]      = {0, 0};

	int antsAmount = 1000;

//	bool  shouldGenerateMap = true;
	float mapGenNoiseScale    = 8.f;
	int   mapGenNoiseBlur     = 2;
	float mapGenNoiseContrast = 8;

	MapGenSettings mapGenSettings = MapGenSettings::FoodAndWalls;

	int mapGenFoodLowThreshold  = 0;
	int mapGenFoodHighThreshold = 64;

	int mapGenWallLowThreshold  = 160;
	int mapGenWallHighThreshold = 255;
};

class Settings
{
	inline static Settings *m_instance;

public:
	Settings()
	{
		assert(m_instance);
		m_instance = this;
	}

	const AntsSettings &GetAntsSettings() const { return m_antsTable; };
	const WorldSettings &GetWorldSettings() const { return m_worldTable; };

	AntsSettings &GetMutableAntsSettings() { return m_antsTable; }
	WorldSettings &GetMutableWorldSettings() { return m_worldTable; }

	void Save(const std::string &filename);
	void Load(const std::string &filename);

	static std::vector<std::string> FindSavedSettings();

	void Reset()
	{
		m_antsTable  = AntsSettings();
		m_worldTable = WorldSettings();
	};

private:
	AntsSettings  m_antsTable;
	WorldSettings m_worldTable;
};

#endif //ANTS_SETTINGS_HPP
