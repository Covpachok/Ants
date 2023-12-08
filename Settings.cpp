#include <iostream>
#include <fstream>
#include <filesystem>

#include "Settings.hpp"

using json = nlohmann::json;

void Settings::Save(const std::string &filename)
{
	std::ofstream f(filename + ".json");
	json          j;

	j["Ants"]            = m_antsSettings;
	j["AntColony"]       = m_antColonySettings;
	j["Global"]          = m_globalSettings;
	j["PheromoneMap"]    = m_pheromoneMapSettings;
	j["TileMap"]         = m_tileMapSettings;
	j["WorldGeneration"] = m_worldGenerationSettings;

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

	m_antsSettings            = data["Ants"];
	m_antColonySettings       = data["AntColony"];
	m_globalSettings          = data["Global"];
	m_pheromoneMapSettings    = data["PheromoneMap"];
	m_tileMapSettings         = data["TileMap"];
	m_worldGenerationSettings = data["WorldGeneration"];

//	m_globalSettings.Recalculate();
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
