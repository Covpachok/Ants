#ifndef ANTS_BRUSH_HPP
#define ANTS_BRUSH_HPP

#include <functional>

#include "Tile.hpp"

class World;

class TileMap;

class Brush
{
public:
	enum BrushType
	{
		Point, Square, Round, Amount
	};

public:
	explicit Brush(TileType paintType = TileType::eFood, BrushType brushType = Round, int brushSize = 5) :
			m_paintType(paintType), m_brushType(brushType), m_brushSize(brushSize) {}

	void Paint(TileMap &tileMap, int x, int y);

	void SetPaintType(TileType type) { m_paintType = type; }
	void SetBrushType(BrushType type) { m_brushType = type; }
	void SetBrushSize(int size) { m_brushSize = size; }

	TileType GetPaintType() const { return m_paintType; }
	BrushType GetBrushType() const { return m_brushType; }
	int GetBrushSize() const { return m_brushSize; }

private:
	TileType  m_paintType;
	BrushType m_brushType;

	int m_brushSize;
};

using BrushType = Brush::BrushType;

#endif //ANTS_BRUSH_HPP
