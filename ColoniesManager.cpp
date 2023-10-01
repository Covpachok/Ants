#include <iostream>
#include "ColoniesManager.hpp"
#include "Settings.hpp"
#include "Random.hpp"

IntVec2 GetRandomNestPos(int nestSize, int width, int height)
{
	return {Random::Int(nestSize + 5, width - nestSize - 5), Random::Int(nestSize + 5, height - nestSize - 5)};
}

ColoniesManager::ColoniesManager(TileMap &tileMap)
{
//	assert(s_instance);
//	s_instance = this;

	m_colonies.resize(k_maxColoniesAmount);
	m_nests.resize(k_maxNestsAmount);

	auto &globalSettings = Settings::Instance().GetGlobalSettings();
	int  width           = static_cast<int>(globalSettings.mapWidth);
	int  height          = static_cast<int>(globalSettings.mapHeight);

	std::cout << "Colony manager map size: " << width << "x" << height << std::endl;

	int nestSize = Settings::Instance().GetAntColonySettings().nestSize;

	std::cout << "Creating colonies";
	for ( size_t i = 0; i < k_maxNestsAmount; ++i )
	{
//		IntVec2 nestPos = GetRandomNestPos(nestSize, width, height);
		IntVec2 nestPos = {width / 2, height / 2};
		Vector2 np; np.x = nestPos.x; np.y = nestPos.y;
		CreateColony(np);//globalSettings.WorldToScreen(nestPos));
		CreateNest(nestPos, tileMap, m_colonies[i].get());
		std::cout << ".";
	}
	std::cout << "\nColonies created." << std::endl;

	std::cout << "Creating empty nests";
	for ( size_t i = s_nextNestId; i < k_maxNestsAmount; ++i )
	{
		IntVec2 nestPos = GetRandomNestPos(nestSize, width, height);
		CreateNest(nestPos, tileMap, nullptr);
		std::cout << ".";
	}
	std::cout << "\nEmpty nests created." << std::endl;
}

void ColoniesManager::CreateColony(const Vector2 &pos)
{
	if ( s_nextColonyId >= k_maxColoniesAmount )
	{
		return;
	}

	m_colonies[s_nextColonyId] = std::make_unique<AntColony>(s_nextColonyId, pos);
	++s_nextColonyId;
}

void ColoniesManager::CreateNest(const IntVec2 &pos, TileMap &tileMap, AntColony *colony)
{
	if ( s_nextNestId >= k_maxNestsAmount )
	{
		return;
	}

	m_nests[s_nextNestId] = std::make_unique<Nest>(s_nextColonyId, colony, pos, tileMap);
	++s_nextNestId;
}
