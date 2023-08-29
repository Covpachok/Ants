#include <iostream>
#include <fstream>
#include <json.hpp>

#include "ValueTable.hpp"

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

void ValueTable::Save(const std::string &filename)
{
	std::ofstream f(filename + ".json");
	json          j;

	j["antsValueTable"]["antMovementSpeed"] = m_antsTable.antMovementSpeed;
	j["antsValueTable"]["antRotationSpeed"] = m_antsTable.antRotationSpeed;

	j["antsValueTable"]["antRandomAngle"] = m_antsTable.antRandomAngle;
	j["antsValueTable"]["antFovRange"]    = m_antsTable.antFovRange;

	j["antsValueTable"]["foodPheromoneStrengthLoss"] = m_antsTable.foodPheromoneStrengthLoss;
	j["antsValueTable"]["homePheromoneStrengthLoss"] = m_antsTable.homePheromoneStrengthLoss;

	j["antsValueTable"]["foodPheromoneIntensity"] = m_antsTable.foodPheromoneIntensity;
	j["antsValueTable"]["homePheromoneIntensity"] = m_antsTable.homePheromoneIntensity;

	j["antsValueTable"]["pheromoneSpawnDelay"] = m_antsTable.pheromoneSpawnDelay;

	j["antsValueTable"]["deviationChance"] = m_antsTable.deviationChance;

	j["antsValueTable"]["antDefaultColor"]  = RayColorToJson(m_antsTable.antDefaultColor);
	j["antsValueTable"]["antWithFoodColor"] = RayColorToJson(m_antsTable.antWithFoodColor);

	// -----

	j["worldValueTable"]["foodPheromoneColor"] = RayColorToJson(m_worldTable.foodPheromoneColor);
	j["worldValueTable"]["homePheromoneColor"] = RayColorToJson(m_worldTable.homePheromoneColor);
	j["worldValueTable"]["foodColor"]          = RayColorToJson(m_worldTable.cellColors[1]);
	j["worldValueTable"]["wallColor"]          = RayColorToJson(m_worldTable.cellColors[2]);

	j["worldValueTable"]["foodAmount"] = m_worldTable.cellDefaultAmount[1];

	j["worldValueTable"]["homePheromoneEvaporationRate"] = m_worldTable.homePheromoneEvaporationRate;
	j["worldValueTable"]["foodPheromoneEvaporationRate"] = m_worldTable.foodPheromoneEvaporationRate;

	j["worldValueTable"]["homeRadius"] = m_worldTable.homeRadius;

	j["worldValueTable"]["centeredHomePos"] = m_worldTable.centeredHomePos;
	j["worldValueTable"]["homePos"]         = m_worldTable.homePos;

	j["worldValueTable"]["antsAmount"] = m_worldTable.antsAmount;

	j["worldValueTable"]["mapGenNoiseScale"]    = m_worldTable.mapGenNoiseScale;
	j["worldValueTable"]["mapGenNoiseBlur"]     = m_worldTable.mapGenNoiseBlur;
	j["worldValueTable"]["mapGenNoiseContrast"] = m_worldTable.mapGenNoiseContrast;

	j["worldValueTable"]["mapGenSettings"] = m_worldTable.mapGenSettings;

	j["worldValueTable"]["mapGenFoodLowThreshold"]  = m_worldTable.mapGenFoodLowThreshold;
	j["worldValueTable"]["mapGenFoodHighThreshold"] = m_worldTable.mapGenFoodHighThreshold;

	j["worldValueTable"]["mapGenWallLowThreshold"]  = m_worldTable.mapGenWallLowThreshold;
	j["worldValueTable"]["mapGenWallHighThreshold"] = m_worldTable.mapGenWallHighThreshold;

	// I hate my life

	f << std::setw(4) << j << std::endl;
}

void ValueTable::Load(const std::string &filename)
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

	m_antsTable.antMovementSpeed = data["antsValueTable"]["antMovementSpeed"];
	m_antsTable.antRotationSpeed = data["antsValueTable"]["antRotationSpeed"];

	m_antsTable.antRandomAngle = data["antsValueTable"]["antRandomAngle"];
	m_antsTable.antFovRange    = data["antsValueTable"]["antFovRange"];

	m_antsTable.foodPheromoneStrengthLoss = data["antsValueTable"]["foodPheromoneStrengthLoss"];
	m_antsTable.homePheromoneStrengthLoss = data["antsValueTable"]["homePheromoneStrengthLoss"];

	m_antsTable.foodPheromoneIntensity = data["antsValueTable"]["foodPheromoneIntensity"];
	m_antsTable.homePheromoneIntensity = data["antsValueTable"]["homePheromoneIntensity"];

	m_antsTable.pheromoneSpawnDelay = data["antsValueTable"]["pheromoneSpawnDelay"];
	m_antsTable.deviationChance     = data["antsValueTable"]["deviationChance"];

	m_antsTable.antDefaultColor  = JsonToRayColor(data["antsValueTable"]["antDefaultColor"]);
	m_antsTable.antWithFoodColor = JsonToRayColor(data["antsValueTable"]["antWithFoodColor"]);

	// ------

	m_worldTable.foodPheromoneColor = JsonToRayColor(data["worldValueTable"]["foodPheromoneColor"]);
	m_worldTable.homePheromoneColor = JsonToRayColor(data["worldValueTable"]["homePheromoneColor"]);

	m_worldTable.cellColors[1] = JsonToRayColor(data["worldValueTable"]["foodColor"]);
	m_worldTable.cellColors[2] = JsonToRayColor(data["worldValueTable"]["wallColor"]);

	m_worldTable.cellDefaultAmount[1] = data["worldValueTable"]["foodAmount"];

	m_worldTable.homePheromoneEvaporationRate = data["worldValueTable"]["homePheromoneEvaporationRate"];
	m_worldTable.foodPheromoneEvaporationRate = data["worldValueTable"]["foodPheromoneEvaporationRate"];

	m_worldTable.homeRadius = data["worldValueTable"]["homeRadius"];

	m_worldTable.centeredHomePos = data["worldValueTable"]["centeredHomePos"];

	m_worldTable.homePos[0] = data["worldValueTable"]["homePos"][0];
	m_worldTable.homePos[1] = data["worldValueTable"]["homePos"][1];

	m_worldTable.antsAmount = data["worldValueTable"]["antsAmount"];

	m_worldTable.mapGenNoiseScale    = data["worldValueTable"]["mapGenNoiseScale"];
	m_worldTable.mapGenNoiseBlur     = data["worldValueTable"]["mapGenNoiseBlur"];
	m_worldTable.mapGenNoiseContrast = data["worldValueTable"]["mapGenNoiseContrast"];

	m_worldTable.mapGenSettings = data["worldValueTable"]["mapGenSettings"];

	m_worldTable.mapGenFoodLowThreshold  = data["worldValueTable"]["mapGenFoodLowThreshold"];
	m_worldTable.mapGenFoodHighThreshold = data["worldValueTable"]["mapGenFoodHighThreshold"];

	m_worldTable.mapGenWallLowThreshold  = data["worldValueTable"]["mapGenWallLowThreshold"];
	m_worldTable.mapGenWallHighThreshold = data["worldValueTable"]["mapGenWallHighThreshold"];
}