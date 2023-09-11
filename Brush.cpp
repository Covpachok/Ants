#include "Brush.hpp"


void PaintPoint(TileMap &tileMap, Brush &brush, int x, int y)
{
	tileMap.SetTile({x, y}, brush.GetPaintType());
}

void PaintSquare(TileMap &tileMap, Brush &brush, int x, int y)
{
	int radius = brush.GetBrushSize();

	for ( int i = -radius; i <= radius; ++i )
	{
		for ( int j = -radius; j <= radius; ++j )
		{
			tileMap.SetTile({j + x, i + y}, brush.GetPaintType());
		}
	}
}

void PaintRound(TileMap &tileMap, Brush &brush, int x, int y)
{
	int radius        = brush.GetBrushSize();
	int radiusSquared = radius * radius;

	for ( int i = -radius; i <= radius; ++i )
	{
		for ( int j = -radius; j <= radius; ++j )
		{
			if ( i * i + j * j <= radiusSquared )
			{
				tileMap.SetTile({j + x, i + y}, brush.GetPaintType());
			}
		}
	}
}

void Brush::Paint(World &world, int x, int y)
{
	auto &tileMap = world.GetTileMap();
	m_paintFunc(tileMap, *this, x, y);
}

const std::function<void(TileMap &, Brush &, int x, int y)> k_paintFuncitons[Brush::Amount] = {PaintPoint, PaintSquare,
                                                                                             PaintRound};

void Brush::OnBrushChanged()
{
	m_paintFunc = k_paintFuncitons[m_brushType];
}