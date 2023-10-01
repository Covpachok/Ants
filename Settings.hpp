#ifndef ANTS_SETTINGS_HPP
#define ANTS_SETTINGS_HPP

#include <raylib.h>
#include <cassert>
#include <string>
#include <vector>

#include <json.hpp>

#include "IntVec.hpp"
#include "Tile.hpp"
#include "WorldGenerator.hpp"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(IntVec2, x, y)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Color, r, g, b, a)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Range, low, high)

enum class TilesGeneration
{
	eNone, eFoodOnly, eWallsOnly, eFoodAndWalls, eAmount
};

NLOHMANN_JSON_SERIALIZE_ENUM(TilesGeneration, {
	{ TilesGeneration::eNone, "none" },
	{ TilesGeneration::eFoodOnly, "foodOnly" },
	{ TilesGeneration::eWallsOnly, "wallsOnly" },
	{ TilesGeneration::eFoodAndWalls, "foodAndWalls" },
	{ TilesGeneration::eAmount, "amount" }
})

struct AntsSettings
{
	float antMovementSpeed        = 30 * 0.016;
	float antRotationSpeed        = 12 * 0.016;
	float antRandomRotation       = 0.3;
	int   antFovRange             = 8;
	float pheromoneStrengthLoss   = 0.00004;
	float pheromoneSpawnIntensity = 128;
	float pheromoneSpawnDelay     = 15;
	float fovCheckDelay           = 3;
	Color antDefaultColor         = {128, 128, 255, 128};
	Color antWithFoodColor        = {128, 255, 128, 128};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AntsSettings,
                                   antMovementSpeed,
                                   antRotationSpeed,
                                   antRandomRotation,
                                   antFovRange,
                                   pheromoneStrengthLoss,
                                   pheromoneSpawnIntensity,
                                   pheromoneSpawnDelay,
                                   fovCheckDelay,
                                   antDefaultColor,
                                   antWithFoodColor)

struct AntColonySettings
{
	size_t coloniesAmount = 1;
	int    foodToSpawnAnt = 10;

	int antsStartAmount = 1000;
	int antsMaxAmount   = 2500;

	float antDeathDelay = 10;

	int nestSize = 5;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AntColonySettings,
                                   coloniesAmount,
                                   foodToSpawnAnt,
                                   antsStartAmount,
                                   antsMaxAmount,
                                   antDeathDelay,
                                   nestSize)

struct GlobalSettings
{
	size_t windowWidth  = 1280;
	size_t windowHeight = 720;

//	float screenToMapRatio        = 1.f;
//	float screenToMapInverseRatio = 1.f / screenToMapRatio;

	size_t mapWidth  = 640; //static_cast<size_t>(static_cast<float>(windowWidth) / screenToMapRatio);
	size_t mapHeight = 360; //static_cast<size_t>(static_cast<float>(windowHeight) / screenToMapRatio);

//	IntVec2 ScreenToWorld(const Vector2 &pos) const
//	{
//		return {pos.x * screenToMapInverseRatio, pos.y * screenToMapInverseRatio};
//	}
//	Vector2 WorldToScreen(const IntVec2 &pos) const
//	{
//		return {static_cast<float>(pos.x) * screenToMapRatio, static_cast<float>(pos.y) * screenToMapRatio};
//	}

//	void Recalculate()
//	{
//		mapWidth  = static_cast<size_t>(static_cast<float>(windowWidth) / screenToMapRatio);
//		mapHeight = static_cast<size_t>(static_cast<float>(windowHeight) / screenToMapRatio);
//	}
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GlobalSettings,
                                   windowWidth,
                                   windowHeight,
								   mapWidth,
								   mapHeight)

struct PheromoneMapSettings
{
	float pheromoneEvaporationRate = 0.005f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PheromoneMapSettings,
                                   pheromoneEvaporationRate)

struct TileMapSettings
{
	std::array<Color, static_cast<size_t>(TileType::eAmount)> tileDefaultColors = {
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

	int foodDefaultAmount = 32;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TileMapSettings,
                                   tileDefaultColors,
                                   foodDefaultAmount)

struct WorldGenerationSettings
{
	TilesGeneration tilesGeneration = TilesGeneration::eFoodAndWalls;

	int   seed            = 1;
	float noiseScale      = 7.f;
	int   noiseOctaves    = 8;
	float noiseContrast   = 2.25f;
	float noiseBlur       = 2.f;
	float ridgesIntensity = 0.6f;

	Range foodSpawnRange;
	Range wallSpawnRange;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WorldGenerationSettings,
                                   tilesGeneration,
                                   seed,
                                   noiseScale,
                                   noiseOctaves,
                                   noiseBlur,
                                   noiseContrast,
                                   ridgesIntensity,
                                   foodSpawnRange,
                                   wallSpawnRange)

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

	// clang-format off
	const AntsSettings              &GetAntsSettings()              const { return m_antsSettings; };
	const AntColonySettings         &GetAntColonySettings()         const { return m_antColonySettings; };
	const GlobalSettings            &GetGlobalSettings()            const { return m_globalSettings; };
	const PheromoneMapSettings      &GetPheromoneMapSettings()      const { return m_pheromoneMapSettings; };
	const TileMapSettings           &GetTileMapSettings()           const { return m_tileMapSettings; };
	const WorldGenerationSettings   &GetWorldGenerationSettings()   const { return m_worldGenerationSettings; };

	AntsSettings            &GetAntsSettings()            { return m_antsSettings; };
	AntColonySettings       &GetAntColonySettings()       { return m_antColonySettings; };
	GlobalSettings          &GetGlobalSettings()          { return m_globalSettings; };
	PheromoneMapSettings    &GetPheromoneMapSettings()    { return m_pheromoneMapSettings; };
	TileMapSettings         &GetTileMapSettings()         { return m_tileMapSettings; };
	WorldGenerationSettings &GetWorldGenerationSettings() { return m_worldGenerationSettings; };
	// clang-format on

	void Save(const std::string &filename);
	void Load(const std::string &filename);

	static std::vector<std::string> FindSavedSettings();

	inline void Reset()
	{
		m_antsSettings            = AntsSettings();
		m_antColonySettings       = AntColonySettings();
		m_globalSettings          = GlobalSettings();
		m_pheromoneMapSettings    = PheromoneMapSettings();
		m_tileMapSettings         = TileMapSettings();
		m_worldGenerationSettings = WorldGenerationSettings();
	};

private:
	AntsSettings            m_antsSettings;
	AntColonySettings       m_antColonySettings;
	GlobalSettings          m_globalSettings;
	PheromoneMapSettings    m_pheromoneMapSettings;
	TileMapSettings         m_tileMapSettings;
	WorldGenerationSettings m_worldGenerationSettings;
};

#endif //ANTS_SETTINGS_HPP
