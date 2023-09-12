#include "Nest.hpp"

#include "PheromoneMap.hpp"
#include "World.hpp"

#include "Settings.hpp"
#include "Brush.hpp"

Nest::Nest(World &world) :
		Nest(world, Settings::Instance().GetWorldSettings().homePos) {}

Nest::Nest(World &world, const IntVec2 &pos)
{
	auto &worldSettings = Settings::Instance().GetWorldSettings();

	m_pos = worldSettings.homePos;
	float ratio = worldSettings.screenToMapRatio;
	m_screenPos = {m_pos.x * ratio, m_pos.y * ratio};

	m_size       = worldSettings.homeRadius;
	m_screenSize = m_size * ratio;

	m_color = worldSettings.homePheromoneColor;

	m_foodStored = 0;

	Relocate(m_pos, world.GetTileMap());
}

void Nest::Relocate(const IntVec2 &newPos, TileMap &tileMap)
{
	Brush brush{TileType::Empty, BrushType::Round, m_size};
	// Clear previous
	brush.Paint(tileMap, m_pos.x, m_pos.y);

	m_pos = newPos;
	brush.SetPaintType(TileType::Nest);
	// Paint new
	brush.Paint(tileMap, m_pos.x, m_pos.y);
}

void Nest::Draw() const
{
	DrawCircleSector(m_screenPos, m_screenSize, 0.f, 360.f, 12, m_color);
}