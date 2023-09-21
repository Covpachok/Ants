#include "ColoniesManager.hpp"
#include "Settings.hpp"
#include "Random.hpp"

ColoniesManager::ColoniesManager(TileMap &tileMap)
{
//	assert(s_instance);
//	s_instance = this;

	m_colonies.resize(k_maxColoniesAmount);
	m_nests.resize(k_maxNestsAmount);

	auto &globalSettings = Settings::Instance().GetGlobalSettings();
	int  width           = static_cast<int>(globalSettings.mapWidth);
	int  height          = static_cast<int>(globalSettings.mapHeight);

	int nestSize = Settings::Instance().GetAntColonySettings().nestSize;

	for ( size_t i = 0; i < k_maxNestsAmount; ++i )
	{
		IntVec2 nestPos = {Random::Int(nestSize, width - nestSize), Random::Int(nestSize, height - nestSize)};
		CreateColony(globalSettings.WorldToScreen(nestPos));
		CreateNest(nestPos, tileMap, m_colonies[i].get());
	}

	for ( size_t i = s_nextNestId; i < k_maxNestsAmount; ++i )
	{
		IntVec2 nestPos = {Random::Int(nestSize, width - nestSize), Random::Int(nestSize, height - nestSize)};
		CreateNest(nestPos, tileMap, nullptr);
	}
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
