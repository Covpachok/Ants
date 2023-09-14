#include "TileMap.hpp"

#include "Settings.hpp"

const IntVec2 k_deltaPos[4] = {
		{1,  0},
		{0,  1},
		{-1, 0},
		{0,  -1}
};

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
	if ( !m_boundsChecker.IsInBounds(pos) || newType == TileType::Nest )
	{
		return;
	}

	m_tiles[pos.y][pos.x]->ChangeType(newType);

	UpdateTileColor(pos);
	for ( int i = 0; i < 4; ++i )
	{
		UpdateTileColor(pos + k_deltaPos[i]);
	}

	UpdateColorMap(pos);
}

void PlaceNest(const Nest &nest)
{

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
			UpdateColorMap({x, y});
		}
	}
}

void TileMap::Draw() const
{
	m_colorMap->Draw();
}

void TileMap::UpdateColorMap(const IntVec2 &pos)
{
	m_colorMap->Set(pos, m_tiles[pos.y][pos.x]->GetColor());
	m_colorMap->UpdatePixel(pos);
}

void TileMap::UpdateTileColor(const IntVec2 &pos)
{
	if ( !m_boundsChecker.IsInBounds(pos))
	{
		return;
	}
	std::array<TileType, 4> neighbors{TileType::Empty, TileType::Empty, TileType::Empty, TileType::Empty};

	for ( int i = 0; i < 4; ++i )
	{
		auto nPos = pos + k_deltaPos[i];
		if ( m_boundsChecker.IsInBounds(nPos))
		{
			neighbors[i] = m_tiles[nPos.y][nPos.x]->GetType();
		}
		else
		{
			neighbors[i] = m_errorTile.GetType();
		}
	}
	m_tiles[pos.y][pos.x]->UpdateColorByNeighbors(neighbors);

	UpdateColorMap(pos);
}
