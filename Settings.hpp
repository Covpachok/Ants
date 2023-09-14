#ifndef ANTS_SETTINGS_HPP
#define ANTS_SETTINGS_HPP

#include <raylib.h>
#include <cassert>
#include <string>
#include <vector>

#include <json.hpp>

#include "IntVec.hpp"
#include "Tile.hpp"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(IntVec2, x, y)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Color, r, g, b, a)

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

struct AntsSettings
{
	float antMovementSpeed  = 40;
	float antRotationSpeed  = 10;
	float antRandomRotation = 0.3;

	int antFovRange = 8;

	float pheromoneStrengthLoss   = 0.005;
	float pheromoneSpawnIntensity = 128;
	float pheromoneSpawnDelay     = 0.25f;

	Color antDefaultColor  = {128, 128, 255, 128};
	Color antWithFoodColor = {128, 255, 128, 128};
};

struct AntColonySettings
{
	size_t coloniesAmount = 1;

	int foodToSpawnAnt = 10;

	int antsStartAmount = 25;
	int antsMaxAmount   = 2500;
};

struct GlobalSettings
{
	size_t windowWidth  = 1280;
	size_t windowHeight = 720;

	float screenToMapRatio        = 2.f;
	float screenToMapInverseRatio = 1.f / screenToMapRatio;

	size_t mapWidth  = static_cast<size_t>(static_cast<float>(windowWidth) / screenToMapRatio);
	size_t mapHeight = static_cast<size_t>(static_cast<float>(windowHeight) / screenToMapRatio);
};

struct PheromoneMapSettings
{
	float homePheromoneEvaporationRate = 0.016f;
	float foodPheromoneEvaporationRate = 0.016f;
};

struct TileMapSettings
{
	std::array<Color, static_cast<size_t>(TileType::Amount)> tileDefaultColors = {
			{
					// Empty
					{0, 0, 0, 255},

					// Wall
					{138, 129, 124, 255},

					// Food
					{132, 248, 44, 255},

					// Nest
					{30, 150, 252, 255}
			}
	};

	int foodDefaultAmount = 30;
};

struct MapGenerationSettings
{
	MapGenSettings mapGenSettings = MapGenSettings::FoodAndWalls;

	float mapGenNoiseScale    = 8.f;
	int   mapGenNoiseBlur     = 2;
	float mapGenNoiseContrast = 8;

	uint8_t mapGenFoodLowThreshold  = 0;
	uint8_t mapGenFoodHighThreshold = 64;

	uint8_t mapGenWallLowThreshold  = 160;
	uint8_t mapGenWallHighThreshold = 255;
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
	AntsSettings          m_antsSettings;
	AntColonySettings     m_antColonySettings;
	GlobalSettings        m_globalSettings;
	PheromoneMapSettings  m_pheromoneMapSettings;
	TileMapSettings       m_tileMapSettings;
	MapGenerationSettings m_mapGenerationSettings;
};

#endif //ANTS_SETTINGS_HPP
