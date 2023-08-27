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

void Brush::OnBrushChanged()
{
	switch ( m_brushType )
	{
		case Point:
			m_paintFunc = PaintPoint;
			break;
		case Square:
			m_paintFunc = PaintSquare;
			break;
		case Round:
			m_paintFunc = PaintRound;
			break;
		default:
			m_paintFunc = PaintPoint;
	}
}
void Brush::SetBrushType(Brush::BrushType type)
{
	if ( type == m_brushType )
	{
		return;
	}

	m_brushType = type;
	OnBrushChanged();
}
