#include "TileMap.hpp"

#include "Settings.hpp"

TileMap::TileMap(int width, int height) :
		m_width(width), m_height(height), m_boundsChecker(0, m_width, 0, m_height)
{
	m_colorMap = std::make_unique<ColorMap>(m_width, m_height, BLACK);

	m_tiles.resize(m_height);
	for ( int y = 0; y < m_height; ++y )
	{
		m_tiles[y].resize(m_width);
		for ( int x = 0; x < m_width; ++x )
		{
			m_tiles[y][x] = std::make_unique<Tile>(IntVec2{x, y}, TileType::Empty);
			m_colorMap->Set(x, y, m_tiles[y][x]->GetColor());
		}
		m_tiles[y].shrink_to_fit();
	}
	m_tiles.shrink_to_fit();

	m_colorMap->Update();
}

void TileMap::SetTile(const IntVec2 &pos, TileType newType)
{
	if ( !m_boundsChecker.IsInBounds(pos))
	{
		return;
	}

	m_tiles[pos.y][pos.x]->ChangeType(newType);
	UpdateColor(pos);
}

void TileMap::TakeFood(const IntVec2 &pos)
{
	if ( !m_boundsChecker.IsInBounds(pos))
	{
		return;
	}

	if ( m_tiles[pos.y][pos.x]->Take())
	{
		SetTile(pos, TileType::Empty);
	}
}

void TileMap::Clear()
{
	for ( int y = 0; y < m_height; ++y )
	{
		for ( int x = 0; x < m_width; ++x )
		{
			m_tiles[y][x]->ChangeType(TileType::Empty);
			UpdateColor({x, y});
		}
	}
}

void TileMap::Draw() const
{
	m_colorMap->Draw();
}

void TileMap::UpdateColor(const IntVec2 &pos)
{
	m_colorMap->Set(pos, m_tiles[pos.y][pos.x]->GetColor());
	m_colorMap->UpdatePixel(pos);
}
