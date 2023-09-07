#include <iostream>
#include <fstream>
#include <filesystem>
#include <json.hpp>

#include "Settings.hpp"

using json = nlohmann::json;

json RayColorToJson(const Color &color)
{
	return json{{"r", color.r},
	            {"g", color.g},
	            {"b", color.b},
	            {"a", color.a}};
}

Color JsonToRayColor(const json &data)
{
	return {data["r"], data["g"], data["b"], data["a"]};
}

void Settings::Save(const std::string &filename)
{
	std::ofstream f(filename + ".json");
	json          j;

	j["antsSettings"]["antMovementSpeed"] = m_antsTable.antMovementSpeed;
	j["antsSettings"]["antRotationSpeed"] = m_antsTable.antRotationSpeed;

	j["antsSettings"]["antRandomRotation"] = m_antsTable.antRandomRotation;
	j["antsSettings"]["antFovRange"]       = m_antsTable.antFovRange;

	j["antsSettings"]["foodPheromoneStrengthLoss"] = m_antsTable.foodPheromoneStrengthLoss;
	j["antsSettings"]["homePheromoneStrengthLoss"] = m_antsTable.homePheromoneStrengthLoss;

	j["antsSettings"]["foodPheromoneIntensity"] = m_antsTable.foodPheromoneIntensity;
	j["antsSettings"]["homePheromoneIntensity"] = m_antsTable.homePheromoneIntensity;

	j["antsSettings"]["pheromoneSpawnDelay"] = m_antsTable.pheromoneSpawnDelay;

	j["antsSettings"]["deviationChance"] = m_antsTable.deviationChance;

	j["antsSettings"]["antDefaultColor"]  = RayColorToJson(m_antsTable.antDefaultColor);
	j["antsSettings"]["antWithFoodColor"] = RayColorToJson(m_antsTable.antWithFoodColor);

	// -----

	j["worldSettings"]["foodPheromoneColor"] = RayColorToJson(m_worldTable.foodPheromoneColor);
	j["worldSettings"]["homePheromoneColor"] = RayColorToJson(m_worldTable.homePheromoneColor);
	j["worldSettings"]["foodColor"]          = RayColorToJson(m_worldTable.tileColors[1]);
	j["worldSettings"]["wallColor"]          = RayColorToJson(m_worldTable.tileColors[2]);

	j["worldSettings"]["foodAmount"] = m_worldTable.tileDefaultAmount[1];

	j["worldSettings"]["homePheromoneEvaporationRate"] = m_worldTable.homePheromoneEvaporationRate;
	j["worldSettings"]["foodPheromoneEvaporationRate"] = m_worldTable.foodPheromoneEvaporationRate;

	j["worldSettings"]["homeRadius"] = m_worldTable.homeRadius;

	j["worldSettings"]["centeredHomePos"] = m_worldTable.centeredHomePos;
	j["worldSettings"]["homePos"]         = m_worldTable.homePos;

	j["worldSettings"]["antsAmount"] = m_worldTable.antsAmount;

	j["worldSettings"]["mapGenNoiseScale"]    = m_worldTable.mapGenNoiseScale;
	j["worldSettings"]["mapGenNoiseBlur"]     = m_worldTable.mapGenNoiseBlur;
	j["worldSettings"]["mapGenNoiseContrast"] = m_worldTable.mapGenNoiseContrast;

	j["worldSettings"]["mapGenSettings"] = m_worldTable.mapGenSettings;

	j["worldSettings"]["mapGenFoodLowThreshold"]  = m_worldTable.mapGenFoodLowThreshold;
	j["worldSettings"]["mapGenFoodHighThreshold"] = m_worldTable.mapGenFoodHighThreshold;

	j["worldSettings"]["mapGenWallLowThreshold"]  = m_worldTable.mapGenWallLowThreshold;
	j["worldSettings"]["mapGenWallHighThreshold"] = m_worldTable.mapGenWallHighThreshold;

	// I hate my life

	f << std::setw(4) << j << std::endl;
}

void Settings::Load(const std::string &filename)
{
	std::ifstream f(filename);
	json          data;
	try
	{
		data = json::parse(f);
	}
	catch ( const json::exception &e )
	{
		std::cout << e.what() << std::endl;
		return;
	}

	std::cout << std::setw(4) << data << std::endl;

	m_antsTable.antMovementSpeed = data["antsSettings"]["antMovementSpeed"];
	m_antsTable.antRotationSpeed = data["antsSettings"]["antRotationSpeed"];

	m_antsTable.antRandomRotation = data["antsSettings"]["antRandomRotation"];
	m_antsTable.antFovRange       = data["antsSettings"]["antFovRange"];

	m_antsTable.foodPheromoneStrengthLoss = data["antsSettings"]["foodPheromoneStrengthLoss"];
	m_antsTable.homePheromoneStrengthLoss = data["antsSettings"]["homePheromoneStrengthLoss"];

	m_antsTable.foodPheromoneIntensity = data["antsSettings"]["foodPheromoneIntensity"];
	m_antsTable.homePheromoneIntensity = data["antsSettings"]["homePheromoneIntensity"];

	m_antsTable.pheromoneSpawnDelay = data["antsSettings"]["pheromoneSpawnDelay"];
	m_antsTable.deviationChance     = data["antsSettings"]["deviationChance"];

	m_antsTable.antDefaultColor  = JsonToRayColor(data["antsSettings"]["antDefaultColor"]);
	m_antsTable.antWithFoodColor = JsonToRayColor(data["antsSettings"]["antWithFoodColor"]);

	// ------

	m_worldTable.foodPheromoneColor = JsonToRayColor(data["worldSettings"]["foodPheromoneColor"]);
	m_worldTable.homePheromoneColor = JsonToRayColor(data["worldSettings"]["homePheromoneColor"]);

	m_worldTable.tileColors[1] = JsonToRayColor(data["worldSettings"]["foodColor"]);
	m_worldTable.tileColors[2] = JsonToRayColor(data["worldSettings"]["wallColor"]);

	m_worldTable.tileDefaultAmount[1] = data["worldSettings"]["foodAmount"];

	m_worldTable.homePheromoneEvaporationRate = data["worldSettings"]["homePheromoneEvaporationRate"];
	m_worldTable.foodPheromoneEvaporationRate = data["worldSettings"]["foodPheromoneEvaporationRate"];

	m_worldTable.homeRadius = data["worldSettings"]["homeRadius"];

	m_worldTable.centeredHomePos = data["worldSettings"]["centeredHomePos"];

	m_worldTable.homePos[0] = data["worldSettings"]["homePos"][0];
	m_worldTable.homePos[1] = data["worldSettings"]["homePos"][1];

	m_worldTable.antsAmount = data["worldSettings"]["antsAmount"];

	m_worldTable.mapGenNoiseScale    = data["worldSettings"]["mapGenNoiseScale"];
	m_worldTable.mapGenNoiseBlur     = data["worldSettings"]["mapGenNoiseBlur"];
	m_worldTable.mapGenNoiseContrast = data["worldSettings"]["mapGenNoiseContrast"];

	m_worldTable.mapGenSettings = data["worldSettings"]["mapGenSettings"];

	m_worldTable.mapGenFoodLowThreshold  = data["worldSettings"]["mapGenFoodLowThreshold"];
	m_worldTable.mapGenFoodHighThreshold = data["worldSettings"]["mapGenFoodHighThreshold"];

	m_worldTable.mapGenWallLowThreshold  = data["worldSettings"]["mapGenWallLowThreshold"];
	m_worldTable.mapGenWallHighThreshold = data["worldSettings"]["mapGenWallHighThreshold"];
}

std::vector<std::string> Settings::FindSavedSettings()
{
	namespace fs = std::filesystem;
	fs::path path = fs::current_path();

	std::vector<std::string> saveFiles;

	for ( const auto &entry: fs::directory_iterator(path))
	{
		if ( fs::is_regular_file(entry))
		{
			if ( entry.path().extension() == ".json" )
			{
				saveFiles.push_back(entry.path().filename().string());
			}
		}
	}

	return saveFiles;
}
