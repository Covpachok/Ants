#include "Nest.hpp"

#include "Settings.hpp"
#include "Brush.hpp"
#include "AntColony.hpp"

Nest::Nest(NestId id, AntColony *colony, const IntVec2 &pos, TileMap &tileMap) :
		m_colony(colony), m_id(id)
{
	auto &globalSettings    = Settings::Instance().GetGlobalSettings();
	auto &antColonySettings = Settings::Instance().GetAntColonySettings();

	m_pos = pos;
	m_screenPos.x = pos.x;
	m_screenPos.y = pos.y;
	//m_screenPos = globalSettings.WorldToScreen(m_pos);

	m_size       = antColonySettings.nestSize;
	m_foodStored = 0;

	tileMap.PlaceNest(*this);
}

void Nest::Relocate(const IntVec2 &newPos, TileMap &tileMap)
{
	tileMap.PlaceNest(*this);
}

void Nest::OnFoodStoredIncrease()
{
	const int foodToSpawnAnt = Settings::Instance().GetAntColonySettings().foodToSpawnAnt;
	if ( m_foodStored >= foodToSpawnAnt && m_colony )
	{
		m_foodStored -= foodToSpawnAnt;
		m_colony->SpawnAnt(m_screenPos);
	}
}
