#ifndef ANTS_SETTINGS_HPP
#define ANTS_SETTINGS_HPP

#include <raylib.h>
#include <cassert>
#include <string>
#include <vector>

#include <json.hpp>

#include "IntVec.hpp"
#include "Tile.hpp"

constexpr int k_tilesAmount = 3;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(IntVec2, x, y)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Color, r, g, b, a)

struct AntsSettings
{
	float antMovementSpeed          = 40;
	float antRotationSpeed          = 10;
	float antRandomRotation         = 0.3;
	int   antFovRange               = 12;
	float foodPheromoneStrengthLoss = 0.005;
	float homePheromoneStrengthLoss = 0.005;
	float foodPheromoneIntensity    = 128;
	float homePheromoneIntensity    = 128;
	float pheromoneSpawnDelay       = 0.25f;
	float deviationChance           = 0.001;
	Color antDefaultColor           = {128, 128, 255, 128};
	Color antWithFoodColor          = {128, 255, 128, 128};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AntsSettings,
                                   antMovementSpeed,
                                   antRotationSpeed,
                                   antRandomRotation,
                                   antFovRange,
                                   foodPheromoneStrengthLoss,
                                   homePheromoneStrengthLoss,
                                   foodPheromoneIntensity,
                                   homePheromoneIntensity,
                                   pheromoneSpawnDelay,
                                   deviationChance,
                                   antDefaultColor,
                                   antWithFoodColor)

enum class MapGenSettings
{
	None, FoodOnly, WallsOnly, FoodAndWalls, Amount
};

NLOHMANN_JSON_SERIALIZE_ENUM(MapGenSettings, {
	{ MapGenSettings::None, "none" },
	{ MapGenSettings::FoodOnly, "foodOnly" },
	{ MapGenSettings::WallsOnly, "wallsOnly" },
	{ MapGenSettings::FoodAndWalls, "foodAndWalls" },
	{ MapGenSettings::Amount, "amount" }
})

struct WorldSettings
{
	int   mapWidth         = 1280 / 2;
	int   mapHeight        = 720 / 2;
	float screenToMapRatio = 3.f;

	Color foodPheromoneColor = {0, 255, 0, 0};
	Color homePheromoneColor = {0, 0, 255, 0};

	std::array<Color, static_cast<size_t>(TileType::Amount)> tileDefaultColors = {
			{{0, 0, 0, 255}, {128, 128, 128, 255}, {0, 255, 0, 255}, {0, 0, 255, 255}}};

	int foodDefaultAmount = 30;

	float homePheromoneEvaporationRate = 0.008f;
	float foodPheromoneEvaporationRate = 0.008f;

	int homeRadius = 5;

	IntVec2 homePos         = {mapWidth / 2, mapHeight / 2};

	int antsAmount = 1000;

//	bool  shouldGenerateMap = true;
	float mapGenNoiseScale    = 8.f;
	int   mapGenNoiseBlur     = 2;
	float mapGenNoiseContrast = 8;

	MapGenSettings mapGenSettings = MapGenSettings::FoodAndWalls;

	uint8_t mapGenFoodLowThreshold  = 0;
	uint8_t mapGenFoodHighThreshold = 64;

	uint8_t mapGenWallLowThreshold  = 160;
	uint8_t mapGenWallHighThreshold = 255;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WorldSettings,
                                   mapWidth,
                                   mapHeight,
                                   screenToMapRatio,
                                   foodPheromoneColor,
                                   homePheromoneColor,
                                   tileDefaultColors,
                                   foodDefaultAmount,
                                   homePheromoneEvaporationRate,
                                   foodPheromoneEvaporationRate,
                                   homeRadius,
                                   homePos,
                                   antsAmount,
                                   mapGenNoiseScale,
                                   mapGenNoiseBlur,
                                   mapGenNoiseContrast,
                                   mapGenSettings,
                                   mapGenFoodLowThreshold,
                                   mapGenFoodHighThreshold,
                                   mapGenWallLowThreshold,
                                   mapGenWallHighThreshold)

class Settings
{
	inline static Settings *m_instance;

public:
	Settings()
	{
		assert(m_instance);
		m_instance = this;
	}

	static const Settings &Instance() { return *m_instance; }

	const AntsSettings &GetAntsSettings() const
	{
		return m_antsTable;
	};
	const WorldSettings &GetWorldSettings() const
	{
		return m_worldTable;
	};

	AntsSettings &GetMutableAntsSettings()
	{
		return m_antsTable;
	}
	WorldSettings &GetMutableWorldSettings()
	{
		return m_worldTable;
	}

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
