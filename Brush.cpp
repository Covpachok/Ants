#include "Brush.hpp"

#include "TileMap.hpp"

void PaintPoint(TileMap &tileMap, Brush &brush, int x, int y)
{
	if ( tileMap.GetTileType({x, y}) == TileType::eNest )
	{
		return;
	}

	tileMap.SetTile({x, y}, brush.GetPaintType());
}

void PaintSquare(TileMap &tileMap, Brush &brush, int x, int y)
{
	int radius = brush.GetBrushSize();

	for ( int i = -radius; i <= radius; ++i )
	{
		for ( int j = -radius; j <= radius; ++j )
		{
			PaintPoint(tileMap, brush, j + x, i + y);
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
				PaintPoint(tileMap, brush, j + x, i + y);
			}
		}
	}
}

void Brush::Paint(TileMap &tileMap, int x, int y)
{
	switch ( m_brushType )
	{
		case Point:
			PaintPoint(tileMap, *this, x, y);
			break;
		case Square:
			PaintSquare(tileMap, *this, x, y);
			break;
		case Round:
			PaintRound(tileMap, *this, x, y);
			break;
		case Amount:
			break;
	}
}