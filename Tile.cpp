#include "Tile.hpp"

#include <bitset>

#include "Settings.hpp"

const std::bitset<Tile::Amount> k_tilesPassability = 0b1100;

Tile::Tile(const IntVec2 &pos, TileType type) :
		m_pos(pos)
{
	ChangeType(type);
}

void Tile::UpdateColor(const std::array<TileType, 4> &neighborTypes)
{
	if ( m_type == TileType::Empty )
	{
		return;
	}

	int sameTypeCount = 0;

	for ( auto &neighborType: neighborTypes )
	{
		sameTypeCount += ( neighborType == m_type );
	}

	float factor = std::min(0.3f * static_cast<float>(sameTypeCount), 1.f);
	m_color = ColorBrightness(GetDefaultColor(), factor);
}

void Tile::ChangeType(TileType type)
{
	m_type = type;

	const auto &settings = Settings::Instance().GetWorldSettings();

	m_color        = settings.tileDefaultColors[static_cast<int>(m_type)];
	m_defaultColor = m_color;

	m_amount = 0;
	if ( m_type == TileType::Food )
	{
		m_amount = settings.foodDefaultAmount;
	}

	m_passable = k_tilesPassability[m_type];
}

bool Tile::Take()
{
	--m_amount;

	if ( m_amount <= 0 )
	{
		return true;
	}
	return false;
}