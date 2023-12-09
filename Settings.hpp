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
#include "AntColony.hpp"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(IntVec2, x, y)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Color, r, g, b, a)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Range, low, high)

struct AntsSettings
{
	float antMovementSpeed  = 0.4f;
	float antRotationSpeed  = 0.25f;//0.25f;
	float antRandomRotation = 0.2f;

	int antFovRange = 8; // Heavily affects performance

	float pheromoneStrengthLoss = 0.00004;

	int deviationDelayMin = 750;
	int deviationDelayMax = 3000;
	int deviationTime     = 200;

	Color antDefaultColor  = {128, 128, 255, 128};
	Color antWithFoodColor = {128, 255, 128, 128};
};


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AntsSettings,
                                   antMovementSpeed,
                                   antRotationSpeed,
                                   antRandomRotation,
                                   antFovRange,
                                   pheromoneStrengthLoss,
                                   deviationDelayMin,
                                   deviationDelayMax,
                                   deviationTime,
                                   antDefaultColor,
                                   antWithFoodColor)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AntColonySettings,
                                   coloniesAmount,
                                   antsStartAmount,
                                   antsMaxAmount,
                                   dynamicLife,
                                   antDeathDelay,
                                   foodToSpawnAnt,
                                   nestSize)

// Maybe MapSettings is better naming?
struct GlobalSettings
{
	size_t mapWidth  = 640;
	size_t mapHeight = 360;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GlobalSettings,
                                   mapWidth,
                                   mapHeight)

struct PheromoneMapSettings
{
	float pheromoneEvaporationRate = 0.015f;
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
	int   seed            = 1;
	float noiseScale      = 7.f;
	int   noiseOctaves    = 8;
	float noiseContrast   = 2.25f;
	float noiseBlur       = 2.f;
	float ridgesIntensity = 0.6f;

	Range foodRange{0.75f, 1.f};
	Range wallRange{0.f, 0.15f};
	Range emptyRange{0.6f, 0.65f};
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WorldGenerationSettings,
                                   seed,
                                   noiseScale,
                                   noiseOctaves,
                                   noiseBlur,
                                   noiseContrast,
                                   ridgesIntensity,
                                   foodRange,
                                   wallRange,
                                   emptyRange)

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
