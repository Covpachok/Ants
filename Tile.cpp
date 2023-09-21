#include "Tile.hpp"

#include "Settings.hpp"

Tile::Tile(const IntVec2 &pos, TileType type) :
		m_pos(pos)
{
	ChangeType(type);
}

void Tile::UpdateColorByNeighbors(const std::array<TileType, 4> &neighborTypes)
{
	if ( m_type == TileType::eEmpty )
	{
		return;
	}

	int sameTypeCount = 0;

	for ( auto &neighborType: neighborTypes )
	{
		sameTypeCount += ( neighborType == m_type );
	}

	float factor = 0.25f + std::min(0.1875f * static_cast<float>(sameTypeCount), 0.75f);
	m_color = {
			static_cast<unsigned char>(m_defaultColor.r * factor),
			static_cast<unsigned char>(m_defaultColor.g * factor),
			static_cast<unsigned char>(m_defaultColor.b * factor),
			m_defaultColor.a
	};
}

void Tile::ChangeType(TileType type, Nest *nest)
{
	m_type = type;

	const auto &settings = Settings::Instance().GetTileMapSettings();

	m_color        = settings.tileDefaultColors[static_cast<int>(m_type)];
	m_defaultColor = m_color;

	m_amount = 0;
	m_nest   = nullptr;
	if ( m_type == TileType::eFood )
	{
		m_amount = settings.foodDefaultAmount;
	}
	else if ( m_type == TileType::eNest )
	{
		m_nest = nest;
	}
}