#include "Nest.hpp"

#include "Settings.hpp"
#include "Brush.hpp"

Nest::Nest(NestId id, TileMap &tileMap, const IntVec2 &pos) :
		m_id(id)
{
	auto &worldSettings = Settings::Instance().GetWorldSettings();

	m_pos        = worldSettings.homePos;
	m_screenPos  = {
			static_cast<float>(m_pos.x) * worldSettings.screenToMapRatio,
			static_cast<float>(m_pos.y) * worldSettings.screenToMapRatio
	};

	m_size       = worldSettings.homeRadius;
	m_foodStored = 0;

	Relocate(m_pos, tileMap);
}

void Nest::Relocate(const IntVec2 &newPos, TileMap &tileMap)
{
	Brush brush{TileType::Empty, BrushType::Round, m_size};
	brush.Paint(tileMap, m_pos.x, m_pos.y);

	m_pos = newPos;
	brush.SetPaintType(TileType::Nest);
	brush.Paint(tileMap, m_pos.x, m_pos.y);
}

void Nest::OnFoodStoredIncrease()
{
	const int foodToSpawnAnt = Settings::Instance().GetAntsSettings().foodToSpawnAnt;
	if ( m_foodStored >= foodToSpawnAnt )
	{
		m_foodStored -= foodToSpawnAnt;
	}

}
