#include "TileMap.hpp"

#include "Settings.hpp"
#include "Brush.hpp"

#include "Nest.hpp"

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
			m_tiles[y][x] = std::make_unique<Tile>(IntVec2{x, y}, TileType::eEmpty);
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

	m_tiles[pos.y][pos.x]->ChangeType(newType, m_nest);

	UpdateTileColor(pos);
	for (auto deltaPos : k_deltaPos)
	{
		UpdateTileColor(pos + deltaPos);
	}

	UpdateColorMap(pos);
}

// Doesn't check bounds, doesn't update colorMap, doesn't color update by neighbours
void TileMap::UnsafeSetTile(int x, int y, TileType newType)
{
	m_tiles[y][x]->ChangeType(newType, m_nest);
//	m_colorMap->Set(x, y, m_tiles[y][x]->GetColor());
}

void TileMap::Update()
{
#pragma omp parallel for collapse(2) default(none)
	for(int y = 0; y < m_height; ++y)
	{
		for(int x = 0; x < m_width; ++x)
		{
			UpdateTileColor({x, y});
		}
	}

	m_colorMap->Update();
}

bool TileMap::TakeFood(const IntVec2 &pos)
{
	if ( !m_boundsChecker.IsInBounds(pos))
	{
		return false;
	}

	bool depleted = m_tiles[pos.y][pos.x]->Take();
	if ( depleted )
	{
		SetTile(pos, TileType::eEmpty);
	}
	return depleted;
}

void TileMap::Clear()
{
	for ( int y = 0; y < m_height; ++y )
	{
		for ( int x = 0; x < m_width; ++x )
		{
			if ( m_tiles[y][x]->GetType() == TileType::eNest )
			{
				continue;
			}

			m_tiles[y][x]->ChangeType(TileType::eEmpty);
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
	std::array<TileType, 4> neighbors{TileType::eEmpty, TileType::eEmpty, TileType::eEmpty, TileType::eEmpty};

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

	m_colorMap->Set(pos, m_tiles[pos.y][pos.x]->GetColor());
}

void TileMap::PlaceNest(Nest &nest)
{
	m_nest = &nest;

	auto  pos = nest.GetPos();
	Brush brush{TileType::eEmpty, BrushType::Round, nest.GetSize() * 2};
	brush.Paint(*this, pos.x, pos.y);

	brush.SetPaintType(TileType::eNest);
	brush.SetBrushSize(nest.GetSize());
	brush.Paint(*this, pos.x, pos.y);

	m_nest = nullptr;
}