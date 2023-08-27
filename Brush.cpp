//
// Created by heylc on 27.08.2023.
//
#include "Brush.hpp"


void PaintPoint(World &world, Brush &brush, int x, int y)
{
	world.SetCell(x, y, brush.GetPaintType());
}

void PaintSquare(World &world, Brush &brush, int x, int y)
{
	int radius = brush.GetBrushSize();

	for ( int i = -radius; i <= radius; ++i )
	{
		for ( int j = -radius; j <= radius; ++j )
		{
			world.SetCell(j + x, i + y, brush.GetPaintType());
		}
	}
}

void PaintRound(World &world, Brush &brush, int x, int y)
{
	int radius        = brush.GetBrushSize();
	int radiusSquared = radius * radius;

	for ( int i = -radius; i <= radius; ++i )
	{
		for ( int j = -radius; j <= radius; ++j )
		{
			if ( i * i + j * j <= radiusSquared )
			{
				world.SetCell(j + x, i + y, brush.GetPaintType());
			}
		}
	}
}

void Brush::Paint(World &world, int x, int y)
{
	m_paintFunc(world, *this, x, y);
}

const std::function<void(World &, Brush &, int x, int y)> k_paintFuncitons[Brush::Amount] = {PaintPoint, PaintSquare,
                                                                                             PaintRound};

void Brush::OnBrushChanged()
{
	m_paintFunc = k_paintFuncitons[m_brushType];
}